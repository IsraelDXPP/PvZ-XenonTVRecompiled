#include "PvZ/SexyAppFramework/Sound/AudiereSoundInstance.h"
#include "PvZ/SexyAppFramework/Sound/AudiereSoundManager.h"
#include <cmath>

namespace Sexy {

AudiereSoundInstance::AudiereSoundInstance(AudiereSoundManager *theManager, OutputStream *theStream)
    : mManager(theManager)
    , mStream(theStream)
    , mLooping(false)
    , mReleased(false)
    , mBasePan(0.0f)
    , mBaseVolume(1.0f)
    , mBasePitch(0.0f)
    , mPan(0.0f)
    , mVolume(1.0f)
    , mPitchAdjust(1.0f)
    , mPlayingFlag(false)
{
    RehupVolume();
}

AudiereSoundInstance::~AudiereSoundInstance() {
    Stop();
    if (mStream) {
        mStream->Release();
        mStream = nullptr;
    }
}

void AudiereSoundInstance::Release() {
    Stop();
    if (mStream) {
        mStream->Release();
        mStream = nullptr;
    }
    mReleased = true;
}

void AudiereSoundInstance::SetBaseVolume(double theVolume) {
    mBaseVolume = static_cast<float>(theVolume);
    RehupVolume();
}

void AudiereSoundInstance::AdjustBasePitch(float thePitch) {
    mBasePitch = thePitch;
    RehupPitch();
}

void AudiereSoundInstance::AdjustPitch(double thePitch) {
    mPitchAdjust = static_cast<float>(thePitch);
    RehupPitch();
}

void AudiereSoundInstance::SetBasePan(int thePan) {
    mBasePan = thePan / 100.0f;
    RehupPan();
}

void AudiereSoundInstance::SetVolume(double theVolume) {
    mVolume = static_cast<float>(theVolume);
    RehupVolume();
}

void AudiereSoundInstance::SetPan(int thePan) {
    mPan = thePan / 100.0f;
    RehupPan();
}

bool AudiereSoundInstance::Play(bool looping, bool autoRelease) {
    Stop();
    mLooping = autoRelease;
    if (!mStream) {
        return false;
    }
    mStream->SetRepeat(looping);
    mStream->Play();
    mPlayingFlag = true;
    return true;
}

void AudiereSoundInstance::Stop() {
    if (mStream) {
        mStream->Stop();
        mStream->SetPosition(0);
    }
    mLooping = false;
}

bool AudiereSoundInstance::IsPlaying() {
    if (!mStream) {
        return false;
    }
    return mStream->IsPlaying();
}

bool AudiereSoundInstance::IsReleased() {
    if (mReleased) {
        return true;
    }
    if (mLooping && mPlayingFlag) {
        if (!IsPlaying()) {
            Release();
        }
    }
    return mReleased;
}

double AudiereSoundInstance::GetVolume() {
    return mVolume;
}

void AudiereSoundInstance::RehupPitch() {
    if (!mStream) {
        return;
    }

    double semitones = static_cast<double>(mBasePitch * mPitchAdjust);

    double ratio = std::pow(1.05946309, semitones);

    if (ratio * 44100.0 < 100.0) {
        ratio = 100.0 / 44100.0;
    }
    if (ratio * 44100.0 > 200000.0) {
        ratio = 200000.0 / 44100.0;
    }

    mStream->SetPitchShift(static_cast<float>(ratio));
}

void AudiereSoundInstance::RehupVolume() {
    if (!mStream) {
        return;
    }

    float masterVol = mManager ? mManager->GetMasterVolumeFloat() : 1.0f;
    float vol = (mBaseVolume * mVolume) * masterVol;
    mStream->SetVolume(vol);
}

void AudiereSoundInstance::RehupPan() {
    if (!mStream) {
        return;
    }

    float pan = mPan + mBasePan;
    if (pan > 1.0f) {
        pan = 1.0f;
    } else if (pan < -1.0f) {
        pan = -1.0f;
    }
    mStream->SetPan(pan);
}

} // namespace Sexy
