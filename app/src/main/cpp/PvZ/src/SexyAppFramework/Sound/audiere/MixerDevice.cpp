#include "PvZ/SexyAppFramework/Sound/audiere/AudioTypes.h"
#include <cstring>
#include <cstdlib>
#include <algorithm>

namespace audiere {

MixerDevice::MixerDevice()
    : mBufferSize(4096)
{
}

MixerDevice::~MixerDevice() {
    std::lock_guard<std::recursive_mutex> lock(mMutex);
    for (auto *stream : mStreams) {
        delete stream;
    }
    mStreams.clear();
}

void MixerDevice::addStream(MixerStream *stream) {
    std::lock_guard<std::recursive_mutex> lock(mMutex);
    mStreams.push_back(stream);
}

void MixerDevice::removeStream(MixerStream *stream) {
    std::lock_guard<std::recursive_mutex> lock(mMutex);
    mStreams.remove(stream);
}

MixerStream *MixerDevice::openStream(SampleSource *source) {
    if (!source) return nullptr;
    auto *stream = new MixerStream(this, source, mBufferSize);
    return stream;
}

int MixerDevice::read(int frames, void *buffer) {
    auto *out = static_cast<int16_t *>(buffer);
    int totalSamples = frames * 2;
    std::memset(out, 0, totalSamples * sizeof(int16_t));
    std::lock_guard<std::recursive_mutex> lock(mMutex);
    if (mStreams.empty()) return 0;
    auto *mixBuf = static_cast<int16_t *>(malloc(totalSamples * sizeof(int16_t)));
    if (!mixBuf) return 0;
    for (auto *stream : mStreams) {
        if (!stream->mPlaying) continue;
        std::memset(mixBuf, 0, totalSamples * sizeof(int16_t));
        int got = stream->read(frames, mixBuf);
        if (got <= 0) continue;
        int sampleCount = got * 2;
        for (int i = 0; i < sampleCount; i++) {
            int sum = out[i] + mixBuf[i];
            if (sum > 32767) sum = 32767;
            if (sum < -32768) sum = -32768;
            out[i] = static_cast<int16_t>(sum);
        }
    }
    free(mixBuf);
    return frames;
}

} // namespace audiere
