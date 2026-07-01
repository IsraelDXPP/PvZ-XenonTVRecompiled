#ifndef PVZ_SEXYAPPFRAMEWORK_SOUND_OUTPUT_STREAM_H
#define PVZ_SEXYAPPFRAMEWORK_SOUND_OUTPUT_STREAM_H

namespace Sexy {

class OutputStream {
public:
    virtual ~OutputStream() = default;

    virtual void Release() = 0;
    virtual void Play() = 0;
    virtual void Stop() = 0;
    virtual bool IsPlaying() = 0;
    virtual void Reset() = 0;
    virtual void SetRepeat(bool repeat) = 0;
    virtual bool GetRepeat() = 0;
    virtual void SetVolume(float volume) = 0;
    virtual float GetVolume() = 0;
    virtual void SetPan(float pan) = 0;
    virtual float GetPan() = 0;
    virtual void SetPitchShift(float shift) = 0;
    virtual float GetPitchShift() = 0;
    virtual int GetLength() = 0;
    virtual void SetPosition(int position) = 0;
    virtual int GetPosition() = 0;
};

class SampleSource {
public:
    virtual ~SampleSource() = default;
    virtual void Release() = 0;
};

class AudioDevice {
public:
    virtual ~AudioDevice() = default;
    virtual void Release() = 0;
    virtual void Update() = 0;
    virtual OutputStream *OpenStream(SampleSource *source) = 0;
};

} // namespace Sexy

#endif // PVZ_SEXYAPPFRAMEWORK_SOUND_OUTPUT_STREAM_H
