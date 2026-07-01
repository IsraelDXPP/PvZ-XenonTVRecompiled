#ifndef PVZ_SEXYAPPFRAMEWORK_SOUND_SOUND_INSTANCE_H
#define PVZ_SEXYAPPFRAMEWORK_SOUND_SOUND_INSTANCE_H

namespace Sexy {

class SoundInstance {
public:
    virtual ~SoundInstance() = default;

    virtual void Release() = 0;
    virtual void SetBaseVolume(double theVolume) = 0;
    virtual void AdjustBasePitch(float thePitch) = 0;
    virtual void AdjustPitch(double thePitch) = 0;
    virtual void SetBasePan(int thePan) = 0;
    virtual void SetVolume(double theVolume) = 0;
    virtual void SetPan(int thePan) = 0;
    virtual bool Play(bool looping, bool autoRelease) = 0;
    virtual void Stop() = 0;
    virtual bool IsPlaying() = 0;
    virtual bool IsReleased() = 0;
    virtual double GetVolume() = 0;
};

} // namespace Sexy

#endif // PVZ_SEXYAPPFRAMEWORK_SOUND_SOUND_INSTANCE_H
