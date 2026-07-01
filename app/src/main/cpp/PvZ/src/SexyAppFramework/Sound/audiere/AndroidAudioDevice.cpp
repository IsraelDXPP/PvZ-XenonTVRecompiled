#include "PvZ/SexyAppFramework/Sound/audiere/AudioTypes.h"
#include "PvZ/Android/Native/AudioOutput.h"
#include <cstring>
#include <thread>
#include <chrono>

namespace audiere {

audiere::AndroidAudioDevice *gActiveDevice = nullptr;

// Global AGAudio-like wrapper using Native::AudioOutput
namespace {
    Native::AudioOutput gAudioOutput;
    bool gAudioInited = false;

    int agAudioInit(int sampleRate, int channels, int bits) {
        if (gAudioInited) return 0;
        if (gAudioOutput.setup(sampleRate, channels, bits)) {
            gAudioInited = true;
            return 0;
        }
        return -1;
    }

    int agAudioWrite(const void *data, int size) {
        return gAudioOutput.write(data, size);
    }

    bool agAudioIsPaused() {
        return false;
    }
}

AndroidAudioDevice::AndroidAudioDevice(int bufferSize)
    : mBufferSize(bufferSize)
    , mMixBuffer(nullptr)
    , mInitialized(false)
{
}

AndroidAudioDevice::~AndroidAudioDevice() {
    if (mInitialized) {
        gAudioOutput.shutdown();
        gAudioInited = false;
        gActiveDevice = nullptr;
    }
    if (mMixBuffer) {
        delete[] mMixBuffer;
        mMixBuffer = nullptr;
    }
}

const char *AndroidAudioDevice::getName() {
    return "android";
}

int AndroidAudioDevice::writeData() {
    if (!mMixBuffer) return 0;
    int frames = mBufferSize / 4;
    int result = read(frames, mMixBuffer);
    if (result > 0) {
        agAudioWrite(mMixBuffer, 4 * result);
    }
    return result;
}

int AndroidAudioDevice::update() {
    if (agAudioIsPaused()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return 0;
    }
    std::this_thread::sleep_for(std::chrono::microseconds(1));
    return writeData();
}

// Factory function
AndroidAudioDevice *CreateAndroidAudioDevice(int bufferSize) {
    int ret = agAudioInit(44100, 2, 16);
    if (ret < 0) return nullptr;
    auto *dev = new AndroidAudioDevice(bufferSize);
    dev->mMixBuffer = new int16_t[bufferSize / 2];
    dev->mInitialized = true;
    gActiveDevice = dev;
    return dev;
}

} // namespace audiere
