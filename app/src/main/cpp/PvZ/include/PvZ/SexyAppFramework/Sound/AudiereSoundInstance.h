#ifndef PVZ_SEXYAPPFRAMEWORK_SOUND_AUDIERE_SOUND_INSTANCE_H
#define PVZ_SEXYAPPFRAMEWORK_SOUND_AUDIERE_SOUND_INSTANCE_H

#include "SoundInstance.h"
#include "OutputStream.h"

namespace Sexy {

class AudiereSoundManager;

class AudiereSoundInstance : public SoundInstance {
public:
    AudiereSoundInstance(AudiereSoundManager *theManager, OutputStream *theStream);
    ~AudiereSoundInstance() override;

    void Release() override;
    void SetBaseVolume(double theVolume) override;
    void AdjustBasePitch(float thePitch) override;
    void AdjustPitch(double thePitch) override;
    void SetBasePan(int thePan) override;
    void SetVolume(double theVolume) override;
    void SetPan(int thePan) override;
    bool Play(bool looping, bool autoRelease) override;
    void Stop() override;
    bool IsPlaying() override;
    bool IsReleased() override;
    double GetVolume() override;

    void RehupPitch();
    void RehupVolume();
    void RehupPan();

    AudiereSoundManager *mManager;
    OutputStream *mStream;
    bool mLooping;
    bool mReleased;
    float mBasePan;
    float mBaseVolume;
    float mBasePitch;
    float mPan;
    float mVolume;
    float mPitchAdjust;
    bool mPlayingFlag;
};

} // namespace Sexy

#endif // PVZ_SEXYAPPFRAMEWORK_SOUND_AUDIERE_SOUND_INSTANCE_H
