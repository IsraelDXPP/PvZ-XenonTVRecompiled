#ifndef PVZ_AUDIERE_AUDIO_TYPES_H
#define PVZ_AUDIERE_AUDIO_TYPES_H

#include <cstdint>
#include <list>
#include <mutex>
#include "PvZ/SexyAppFramework/Sound/OutputStream.h"

namespace audiere {

class SampleSource {
public:
    virtual ~SampleSource() = default;
    virtual void ref() = 0;
    virtual void unref() = 0;
    virtual int read(int samples, void *buffer) = 0;
    virtual void reset() = 0;
    virtual int getPosition() = 0;
    virtual int setPosition(int position) = 0;
    virtual int getLength() = 0;
    virtual bool isSeekable() = 0;
    virtual int getSampleRate() = 0;
    virtual int getChannels() = 0;
    virtual int getBitsPerSample() = 0;
};

// WAV file loaded in memory
class WAVSource : public SampleSource {
public:
    uint8_t *mData;
    int mDataSize;
    int mSampleRate;
    int mChannels;
    int mBitsPerSample;
    int mPosition;
    int mRefCount;

    WAVSource();
    ~WAVSource() override;

    void ref() override;
    void unref() override;
    int read(int samples, void *buffer) override;
    void reset() override;
    int getPosition() override;
    int setPosition(int position) override;
    int getLength() override;
    bool isSeekable() override;
    int getSampleRate() override;
    int getChannels() override;
    int getBitsPerSample() override;

    bool loadWAV(const uint8_t *data, int size);
    bool loadPCM(const uint8_t *data, int byteSize, int sampleRate, int channels, int bitsPerSample);
};

// Resampler wraps a SampleSource for pitch shifting
class Resampler : public SampleSource {
public:
    struct DumbResampler {
        int32_t field0;
        int32_t field1;
        int32_t field2;
        int32_t field3;
        int32_t field4;
        int32_t field5;
        float field6;
        float field7;
        int32_t field8;
        int32_t field9;
        int32_t field10;
        int32_t field11;
        int32_t field12;
        int32_t field13;
    };

    SampleSource *mSource;
    int mSampleRate;
    DumbResampler *mResampler;
    float mPitchShift;
    int mRefCount;

    Resampler(SampleSource *source, int sampleRate);
    ~Resampler() override;

    void ref() override;
    void unref() override;
    int read(int samples, void *buffer) override;
    void reset() override;
    int getPosition() override;
    int setPosition(int position) override;
    int getLength() override;
    bool isSeekable() override;
    int getSampleRate() override;
    int getChannels() override;
    int getBitsPerSample() override;

    void setPitchShift(float shift);
    float getPitchShift();
    int fillBuffers();
};

// MixerStream implements OutputStream behavior
class MixerDevice;

class MixerStream : public Sexy::OutputStream {
public:
    MixerDevice *mDevice;
    SampleSource *mSource;
    Resampler *mResampler;
    int mVolume;       // 0-255
    int mPan;          // -255 to 255
    bool mPlaying;
    bool mRepeat;
    int mPosition;

    MixerStream(MixerDevice *device, SampleSource *source, int bufferSize);
    ~MixerStream() override;

    int read(int samples, int16_t *buffer);
    void play();
    void stop();
    int isPlaying();
    void setVolume(float vol);
    float getVolume();
    void setPan(float pan);
    float getPan();
    void setPitchShift(float shift);
    float getPitchShift();
    void setRepeat(bool repeat);
    bool getRepeat();
    int getPosition();
    int setPosition(int position);
    int getLength();
    bool isSeekable();
    void reset();

    // OutputStream overrides
    void Release() override;
    void Play() override;
    void Stop() override;
    bool IsPlaying() override;
    void Reset() override;
    void SetRepeat(bool repeat) override;
    bool GetRepeat() override;
    void SetVolume(float volume) override;
    float GetVolume() override;
    void SetPan(float pan) override;
    float GetPan() override;
    void SetPitchShift(float shift) override;
    float GetPitchShift() override;
    int GetLength() override;
    void SetPosition(int position) override;
    int GetPosition() override;
};

// MixerDevice manages and mixes MixerStreams
class MixerDevice {
public:
    std::recursive_mutex mMutex;
    std::list<MixerStream *> mStreams;
    int mBufferSize;

    MixerDevice();
    ~MixerDevice();

    int read(int frames, void *buffer);
    void addStream(MixerStream *stream);
    void removeStream(MixerStream *stream);
    MixerStream *openStream(SampleSource *source);
};

// AndroidAudioDevice uses Native::AudioOutput for playback
class AndroidAudioDevice : public MixerDevice {
public:
    int mBufferSize;
    int16_t *mMixBuffer;
    bool mInitialized;

    AndroidAudioDevice(int bufferSize);
    ~AndroidAudioDevice();

    const char *getName();
    int writeData();
    int update();
};

// Factory function
AndroidAudioDevice *CreateAndroidAudioDevice(int bufferSize);

} // namespace audiere

#endif // PVZ_AUDIERE_AUDIO_TYPES_H
