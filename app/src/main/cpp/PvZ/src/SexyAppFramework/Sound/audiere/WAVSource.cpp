#include "PvZ/SexyAppFramework/Sound/audiere/AudioTypes.h"
#include <cstring>

namespace audiere {

WAVSource::WAVSource()
    : mData(nullptr)
    , mDataSize(0)
    , mSampleRate(0)
    , mChannels(0)
    , mBitsPerSample(0)
    , mPosition(0)
    , mRefCount(1)
{
}

WAVSource::~WAVSource() {
    if (mData) {
        delete[] mData;
        mData = nullptr;
    }
}

void WAVSource::ref() { mRefCount++; }
void WAVSource::unref() { if (--mRefCount <= 0) delete this; }

int WAVSource::read(int samples, void *buffer) {
    if (!mData || mDataSize <= 0) return 0;
    int bytesPerSample = (mBitsPerSample / 8) * mChannels;
    int availableBytes = mDataSize - mPosition;
    int requestedBytes = samples * bytesPerSample;
    int bytesToCopy = requestedBytes < availableBytes ? requestedBytes : availableBytes;
    if (bytesToCopy <= 0) return 0;
    std::memcpy(buffer, mData + mPosition, bytesToCopy);
    mPosition += bytesToCopy;
    return bytesToCopy / bytesPerSample;
}

void WAVSource::reset() { mPosition = 0; }
int WAVSource::getPosition() { return mPosition; }

int WAVSource::setPosition(int position) {
    if (position >= 0 && position <= mDataSize) {
        mPosition = position;
    }
    return mPosition;
}

int WAVSource::getLength() { return mDataSize; }
bool WAVSource::isSeekable() { return true; }
int WAVSource::getSampleRate() { return mSampleRate; }
int WAVSource::getChannels() { return mChannels; }
int WAVSource::getBitsPerSample() { return mBitsPerSample; }

bool WAVSource::loadWAV(const uint8_t *data, int size) {
    if (size < 44) return false;
    if (std::memcmp(data, "RIFF", 4) != 0) return false;
    if (std::memcmp(data + 8, "WAVE", 4) != 0) return false;
    int fmtChunkStart = 12;
    while (fmtChunkStart + 8 < size) {
        if (std::memcmp(data + fmtChunkStart, "fmt ", 4) == 0) break;
        int chunkSize = data[fmtChunkStart + 4] |
                       (data[fmtChunkStart + 5] << 8) |
                       (data[fmtChunkStart + 6] << 16) |
                       (data[fmtChunkStart + 7] << 24);
        fmtChunkStart += 8 + chunkSize;
    }
    if (fmtChunkStart + 24 > size) return false;
    int audioFormat = data[fmtChunkStart + 8] | (data[fmtChunkStart + 9] << 8);
    if (audioFormat != 1) return false;
    mChannels = data[fmtChunkStart + 10] | (data[fmtChunkStart + 11] << 8);
    mSampleRate = data[fmtChunkStart + 12] |
                 (data[fmtChunkStart + 13] << 8) |
                 (data[fmtChunkStart + 14] << 16) |
                 (data[fmtChunkStart + 15] << 24);
    mBitsPerSample = data[fmtChunkStart + 22] | (data[fmtChunkStart + 23] << 8);
    int dataChunkStart = fmtChunkStart + 8 +
                        (data[fmtChunkStart + 4] |
                         (data[fmtChunkStart + 5] << 8) |
                         (data[fmtChunkStart + 6] << 16) |
                         (data[fmtChunkStart + 7] << 24));
    while (dataChunkStart + 8 < size) {
        if (std::memcmp(data + dataChunkStart, "data", 4) == 0) break;
        int chunkSize = data[dataChunkStart + 4] |
                       (data[dataChunkStart + 5] << 8) |
                       (data[dataChunkStart + 6] << 16) |
                       (data[dataChunkStart + 7] << 24);
        dataChunkStart += 8 + chunkSize;
    }
    if (dataChunkStart + 8 > size) return false;
    mDataSize = data[dataChunkStart + 4] |
                (data[dataChunkStart + 5] << 8) |
                (data[dataChunkStart + 6] << 16) |
                (data[dataChunkStart + 7] << 24);
    if (dataChunkStart + 8 + mDataSize > size) return false;
    mData = new uint8_t[mDataSize];
    std::memcpy(mData, data + dataChunkStart + 8, mDataSize);
    mPosition = 0;
    return true;
}

bool WAVSource::loadPCM(const uint8_t *data, int byteSize, int sampleRate, int channels, int bitsPerSample) {
    if (!data || byteSize <= 0 || sampleRate <= 0 || channels <= 0 || channels > 2) return false;
    if (bitsPerSample != 8 && bitsPerSample != 16) return false;

    mData = new uint8_t[byteSize];
    std::memcpy(mData, data, byteSize);
    mDataSize = byteSize;
    mSampleRate = sampleRate;
    mChannels = channels;
    mBitsPerSample = bitsPerSample;
    mPosition = 0;
    return true;
}

} // namespace audiere
