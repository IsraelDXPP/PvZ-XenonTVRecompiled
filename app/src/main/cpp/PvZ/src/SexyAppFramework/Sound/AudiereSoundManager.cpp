#include "PvZ/SexyAppFramework/Sound/AudiereSoundManager.h"
#include "PvZ/SexyAppFramework/Sound/AudiereSoundInstance.h"
#include "PvZ/Symbols.h"
#include <android/log.h>
#include <cstring>
#include <cstdio>

namespace Sexy {

AudiereSoundInfo::AudiereSoundInfo()
    : mSource(nullptr)
    , mDataSize(0)
    , mSampleRate(0)
    , mChannels(0)
    , mBitsPerSample(0)
{
}

AudiereSoundManager::AudiereSoundManager()
    : mMasterVolume(1.0f)
    , mAudioDevice(nullptr)
{
    for (int i = 0; i < MAX_SOUNDS; i++) {
        mSoundInfo[i] = AudiereSoundInfo();
        mBaseVolume[i] = 1.0f;
        mBasePan[i] = 0;
        mBasePitch[i] = 1.0f;
    }
    for (int i = 0; i < MAX_CHANNELS; i++) {
        mChannels[i] = nullptr;
        mChannelIds[i] = -1;
    }
}

AudiereSoundManager::~AudiereSoundManager() {
    ReleaseChannels();
    if (mAudioDevice) {
        delete mAudioDevice;
        mAudioDevice = nullptr;
    }
}

int AudiereSoundManager::LoadSound(const pvzstl::string &theFileName) {
    int id = GetFreeSoundId();
    if (id < 0) return -1;

    FILE *f = std::fopen(theFileName.c_str(), "rb");
    if (!f) return -1;

    std::fseek(f, 0, SEEK_END);
    long fileSize = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);

    auto *data = new uint8_t[fileSize];
    std::fread(data, 1, fileSize, f);
    std::fclose(f);

    // Parse WAV
    auto *wavSource = new audiere::WAVSource();
    if (!wavSource->loadWAV(data, fileSize)) {
        delete[] data;
        delete wavSource;
        return -1;
    }
    delete[] data; // WAVSource copies the data internally

    mSoundInfo[id].mSource = wavSource;
    mSoundInfo[id].mDataSize = wavSource->getLength();
    mSoundInfo[id].mSampleRate = wavSource->getSampleRate();
    mSoundInfo[id].mChannels = wavSource->getChannels();
    mSoundInfo[id].mBitsPerSample = wavSource->getBitsPerSample();

    return id;
}

int AudiereSoundManager::LoadSound(int theSfxID, const pvzstl::string &theFileName) {
    if (theSfxID < 0 || theSfxID >= MAX_SOUNDS) return -1;
    if (mSoundInfo[theSfxID].mSource != nullptr) return theSfxID; // already loaded

    FILE *f = std::fopen(theFileName.c_str(), "rb");
    if (!f) return -1;

    std::fseek(f, 0, SEEK_END);
    long fileSize = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);

    auto *data = new uint8_t[fileSize];
    std::fread(data, 1, fileSize, f);
    std::fclose(f);

    auto *wavSource = new audiere::WAVSource();
    if (!wavSource->loadWAV(data, fileSize)) {
        delete[] data;
        delete wavSource;
        return -1;
    }
    delete[] data;

    mSoundInfo[theSfxID].mSource = wavSource;
    mSoundInfo[theSfxID].mDataSize = wavSource->getLength();
    mSoundInfo[theSfxID].mSampleRate = wavSource->getSampleRate();
    mSoundInfo[theSfxID].mChannels = wavSource->getChannels();
    mSoundInfo[theSfxID].mBitsPerSample = wavSource->getBitsPerSample();

    return theSfxID;
}

int AudiereSoundManager::GetFreeSoundId() {
    for (int i = 0; i < MAX_SOUNDS; i++) {
        if (mSoundInfo[i].mSource == nullptr) {
            return i;
        }
    }
    return -1;
}

int AudiereSoundManager::GetNumSounds() {
    int count = 0;
    for (int i = 0; i < MAX_SOUNDS; i++) {
        if (mSoundInfo[i].mSource != nullptr) {
            count++;
        }
    }
    return count;
}

void AudiereSoundManager::StopAllSounds() {
    for (int i = 0; i < MAX_CHANNELS; i++) {
        if (mChannels[i] != nullptr) {
            bool wasLooping = mChannels[i]->mLooping;
            mChannels[i]->Stop();
            mChannels[i]->mLooping = wasLooping;
        }
    }
}

SoundInstance *AudiereSoundManager::GetSoundInstance(int theSfxID) {
    if (theSfxID < 0 || theSfxID >= MAX_SOUNDS) {
        return nullptr;
    }

    int freeChannel = FindFreeChannel();
    if (freeChannel < 0) {
        return nullptr;
    }

    AudiereSoundInstance *instance = nullptr;
    audiere::SampleSource *source = mSoundInfo[theSfxID].mSource;

    if (mAudioDevice && source) {
        audiere::MixerStream *stream = mAudioDevice->openStream(source);
        if (stream) {
            stream->setVolume(1.0f);
            stream->setPan(0.0f);
            instance = new AudiereSoundInstance(this, static_cast<OutputStream *>(stream));
        }
    }

    if (!instance) {
        instance = new AudiereSoundInstance(this, nullptr);
    }

    mChannels[freeChannel] = instance;
    mChannelIds[freeChannel] = theSfxID;

    instance->SetBaseVolume(mBaseVolume[theSfxID]);
    instance->AdjustBasePitch(mBasePitch[theSfxID]);
    instance->SetBasePan(mBasePan[theSfxID]);

    return instance;
}

void AudiereSoundManager::SetVolume(double theVolume) {
    mMasterVolume = static_cast<float>(theVolume);
    for (int i = 0; i < MAX_CHANNELS; i++) {
        if (mChannels[i] != nullptr) {
            mChannels[i]->RehupVolume();
        }
    }
}

void AudiereSoundManager::SetMasterVolume(double theVolume) {
    mMasterVolume = static_cast<float>(theVolume);
    for (int i = 0; i < MAX_CHANNELS; i++) {
        if (mChannels[i] != nullptr) {
            mChannels[i]->RehupVolume();
        }
    }
}

double AudiereSoundManager::GetMasterVolume() {
    return mMasterVolume;
}

int AudiereSoundManager::SetBaseVolume(int theSfxID, double theBase) {
    if (theSfxID < 0 || theSfxID >= MAX_SOUNDS) {
        return 0;
    }
    mBaseVolume[theSfxID] = static_cast<float>(theBase);
    return 1;
}

int AudiereSoundManager::SetBasePan(int theSfxID, int thePan) {
    if (theSfxID < 0 || theSfxID >= MAX_SOUNDS) {
        return 0;
    }
    mBasePan[theSfxID] = thePan;
    return 1;
}

int AudiereSoundManager::SetBasePitch(int theSfxID, float thePitch) {
    if (theSfxID < 0 || theSfxID >= MAX_SOUNDS) {
        return 0;
    }
    mBasePitch[theSfxID] = thePitch;
    return 1;
}

bool AudiereSoundManager::IsPlaying(int theSfxID) {
    if (theSfxID < 0 || theSfxID >= MAX_SOUNDS) {
        return false;
    }
    for (int i = 0; i < MAX_CHANNELS; i++) {
        if (mChannels[i] != nullptr && mChannelIds[i] == theSfxID && mChannels[i]->IsPlaying()) {
            return true;
        }
    }
    return false;
}

SoundInstance *AudiereSoundManager::GetPlayingSoundInstance(int theSfxID) {
    if (theSfxID < 0 || theSfxID >= MAX_SOUNDS) {
        return nullptr;
    }
    for (int i = 0; i < MAX_CHANNELS; i++) {
        if (mChannels[i] != nullptr && mChannelIds[i] == theSfxID) {
            if (mChannels[i]->IsPlaying()) {
                return mChannels[i];
            }
        }
    }
    return nullptr;
}

void AudiereSoundManager::ReleaseChannels() {
    for (int i = 0; i < MAX_CHANNELS; i++) {
        if (mChannels[i] != nullptr) {
            mChannels[i]->Release();
            delete mChannels[i];
            mChannels[i] = nullptr;
            mChannelIds[i] = -1;
        }
    }
}

void AudiereSoundManager::ReleaseFreeChannels() {
    for (int i = 0; i < MAX_CHANNELS; i++) {
        if (mChannels[i] != nullptr && mChannels[i]->IsReleased()) {
            delete mChannels[i];
            mChannels[i] = nullptr;
            mChannelIds[i] = -1;
        }
    }
}

bool AudiereSoundManager::Initialized() {
    return mAudioDevice != nullptr;
}

int AudiereSoundManager::FindFreeChannel() {
    for (int i = 0; i < MAX_CHANNELS; i++) {
        if (mChannels[i] == nullptr) {
            return i;
        }
        if (mChannels[i]->IsReleased()) {
            delete mChannels[i];
            mChannels[i] = nullptr;
            mChannelIds[i] = -1;
            return i;
        }
    }
    return -1;
}

void AudiereSoundManager::ImportSoundFromBinary(int i) {
    if (!Sexy::gSexyAppBase) return;
    if (mSoundInfo[i].mSource != nullptr) return;

    auto *binarySM = *reinterpret_cast<char **>(
        reinterpret_cast<char *>(Sexy::gSexyAppBase) + 1160);
    if (!binarySM) return;

    void *source = *reinterpret_cast<void **>(binarySM + 4 * i + 4);
    if (!source) return;

    char *info = binarySM + 20 * i + 1028;
    int totalFrames   = *reinterpret_cast<int *>(info);
    int channels      = *reinterpret_cast<int *>(info + 4);
    int sampleRate    = *reinterpret_cast<int *>(info + 8);
    int format        = *reinterpret_cast<int *>(info + 12);
    void *pcmBuffer   = *reinterpret_cast<void **>(info + 16);

    __android_log_print(ANDROID_LOG_DEBUG, "Homura", "ImportSound[%d]: source=%p frames=%d rate=%d ch=%d fmt=%d pcm=%p",
         i, source, totalFrames, sampleRate, channels, format, pcmBuffer);

    int bitsPerSample = (format == 0) ? 8 : 16;
    int pcmByteSize = 0;
    const uint8_t *pcmData = nullptr;
    bool allocated = false;

    if (pcmBuffer && totalFrames > 0 && totalFrames < 10000000 && channels > 0 && channels <= 2) {
        pcmByteSize = totalFrames * channels * (bitsPerSample / 8);
        if (pcmByteSize > 0 && pcmByteSize < 50000000) {
            pcmData = static_cast<const uint8_t *>(pcmBuffer);
        }
    } else if (totalFrames > 0 && totalFrames < 10000000 && channels > 0 && channels <= 2) {
        void **vtbl = *reinterpret_cast<void ***>(source);
        if (vtbl) {
            using ReadFn = int (*)(void *, int, void *);
            ReadFn readFn = reinterpret_cast<ReadFn>(vtbl[3]);
            int readBitsPerSample = (format == 0) ? 8 : 16;
            int readChannels = channels;
            pcmByteSize = totalFrames * readChannels * (readBitsPerSample / 8);
            if (pcmByteSize > 0 && pcmByteSize < 50000000) {
                // Reset WAVInputStream position before reading (binary may have
                // advanced it to end via old_PlaySample). Layout:
                //   +0x10: position (int), +0x30: remainingFrames (int)
                auto *srcBytes = static_cast<char *>(source);
                *reinterpret_cast<int *>(srcBytes + 0x10) = 0;
                *reinterpret_cast<int *>(srcBytes + 0x30) = totalFrames;
                auto *buf = new uint8_t[pcmByteSize];
                int framesRead = readFn(source, totalFrames, buf);
                if (framesRead > 0) {
                    pcmData = buf;
                    allocated = true;
                    totalFrames = framesRead;
                } else {
                    delete[] buf;
                }
            }
        }
    }

    if (!pcmData || pcmByteSize <= 0) return;

    auto *wavSource = new audiere::WAVSource();
    bool ok = wavSource->loadPCM(pcmData, pcmByteSize, sampleRate, channels, bitsPerSample);
    if (allocated) delete[] pcmData;
    if (ok) {
        mSoundInfo[i].mSource = wavSource;
        mSoundInfo[i].mDataSize = totalFrames;
        mSoundInfo[i].mSampleRate = sampleRate;
        mSoundInfo[i].mChannels = channels;
        mSoundInfo[i].mBitsPerSample = bitsPerSample;
    } else {
        delete wavSource;
    }
}

void AudiereSoundManager::ImportFromBinary() {
    if (!Sexy::gSexyAppBase) return;
    auto *binarySM = *reinterpret_cast<char **>(
        reinterpret_cast<char *>(Sexy::gSexyAppBase) + 1160);
    if (!binarySM) return;
    for (int i = 0; i < MAX_SOUNDS; i++) {
        ImportSoundFromBinary(i);
    }
}

} // namespace Sexy
