#include "PvZ/SexyAppFramework/Sound/audiere/AudioTypes.h"
#include <cstring>
#include <algorithm>

namespace audiere {

MixerStream::MixerStream(MixerDevice *device, SampleSource *source, int bufferSize)
    : mDevice(device)
    , mSource(nullptr)
    , mResampler(nullptr)
    , mVolume(255)
    , mPan(0)
    , mPlaying(false)
    , mRepeat(false)
    , mPosition(0)
{
    if (source) {
        mResampler = new Resampler(source, source->getSampleRate());
        mSource = source;
        source->ref();
    }
    if (device) device->addStream(this);
}

MixerStream::~MixerStream() {
    if (mDevice) mDevice->removeStream(this);
    if (mResampler) {
        delete mResampler;
        mResampler = nullptr;
    }
    if (mSource) {
        mSource->unref();
        mSource = nullptr;
    }
}

int MixerStream::read(int samples, int16_t *buffer) {
    if (!mResampler || !mPlaying) return 0;
    std::memset(buffer, 0, samples * sizeof(int16_t) * 2);
    int read_samples = samples;
    int got = mResampler->read(read_samples, buffer);
    if (got <= 0) {
        if (mRepeat) {
            mResampler->reset();
            mPosition = 0;
            got = mResampler->read(read_samples, buffer);
            if (got <= 0) return 0;
        } else {
            mPlaying = false;
            return 0;
        }
    }
    mPosition += got;
    // Apply volume and pan
    if (mVolume != 255 || mPan != 0) {
        float volLeft = 1.0f, volRight = 1.0f;
        if (mPan < 0) {
            volRight = 1.0f + mPan / 255.0f;
        } else if (mPan > 0) {
            volLeft = 1.0f - mPan / 255.0f;
        }
        float vol = mVolume / 255.0f;
        for (int i = 0; i < got; i++) {
            int idx = i * 2;
            int16_t l = static_cast<int16_t>(buffer[idx] * vol * volLeft);
            int16_t r = static_cast<int16_t>(buffer[idx + 1] * vol * volRight);
            buffer[idx] = l;
            buffer[idx + 1] = r;
        }
    }
    return got;
}

void MixerStream::play() {
    if (mDevice) {
        std::lock_guard<std::recursive_mutex> lock(mDevice->mMutex);
        mPlaying = true;
    } else {
        mPlaying = true;
    }
}

void MixerStream::stop() {
    if (mDevice) {
        std::lock_guard<std::recursive_mutex> lock(mDevice->mMutex);
        mPlaying = false;
    } else {
        mPlaying = false;
    }
}

int MixerStream::isPlaying() { return mPlaying ? 1 : 0; }

void MixerStream::setVolume(float vol) {
    mVolume = static_cast<int>(vol * 255.0f + 0.5f);
    if (mVolume > 255) mVolume = 255;
    if (mVolume < 0) mVolume = 0;
}

float MixerStream::getVolume() { return mVolume / 255.0f; }

void MixerStream::setPan(float pan) {
    mPan = static_cast<int>(pan * 255.0f);
    if (mPan > 255) mPan = 255;
    if (mPan < -255) mPan = -255;
}

float MixerStream::getPan() { return mPan / 255.0f; }

void MixerStream::setPitchShift(float shift) {
    if (mResampler) mResampler->setPitchShift(shift);
}

float MixerStream::getPitchShift() {
    return mResampler ? mResampler->getPitchShift() : 1.0f;
}

void MixerStream::setRepeat(bool repeat) { mRepeat = repeat; }
bool MixerStream::getRepeat() { return mRepeat; }
int MixerStream::getPosition() { return mPosition; }

int MixerStream::setPosition(int position) {
    mPosition = position;
    if (mResampler) mResampler->setPosition(position);
    return mPosition;
}

int MixerStream::getLength() { return mResampler ? mResampler->getLength() : 0; }
bool MixerStream::isSeekable() { return mResampler ? mResampler->isSeekable() : false; }

void MixerStream::reset() {
    mPosition = 0;
    if (mResampler) mResampler->reset();
}

// OutputStream overrides
void MixerStream::Release() { stop(); }
void MixerStream::Play() { play(); }
void MixerStream::Stop() { stop(); }
bool MixerStream::IsPlaying() { return isPlaying() != 0; }
void MixerStream::Reset() { reset(); }
void MixerStream::SetRepeat(bool repeat) { setRepeat(repeat); }
bool MixerStream::GetRepeat() { return getRepeat(); }
void MixerStream::SetVolume(float volume) { setVolume(volume); }
float MixerStream::GetVolume() { return getVolume(); }
void MixerStream::SetPan(float pan) { setPan(pan); }
float MixerStream::GetPan() { return getPan(); }
void MixerStream::SetPitchShift(float shift) { setPitchShift(shift); }
float MixerStream::GetPitchShift() { return getPitchShift(); }
int MixerStream::GetLength() { return getLength(); }
void MixerStream::SetPosition(int position) { setPosition(position); }
int MixerStream::GetPosition() { return getPosition(); }

} // namespace audiere
