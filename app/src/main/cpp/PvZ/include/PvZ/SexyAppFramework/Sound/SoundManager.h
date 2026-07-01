#ifndef PVZ_SEXYAPPFRAMEWORK_SOUND_SOUND_MANAGER_H
#define PVZ_SEXYAPPFRAMEWORK_SOUND_SOUND_MANAGER_H

#include "PvZ/STL/pvzstl_string.h"
#include "SoundInstance.h"

namespace Sexy {

class SoundManager {
public:
    virtual ~SoundManager() = default;

    virtual int LoadSound(const pvzstl::string &theFileName) = 0;
    virtual int LoadSound(int theSfxID, const pvzstl::string &theFileName) { return -1; }
    virtual int GetFreeSoundId() = 0;
    virtual int GetNumSounds() = 0;
    virtual void StopAllSounds() = 0;
    virtual SoundInstance *GetSoundInstance(int theSfxID) = 0;
    virtual void SetVolume(double theVolume) = 0;
    virtual void SetMasterVolume(double theVolume) = 0;
    virtual double GetMasterVolume() = 0;
    virtual int SetBaseVolume(int theSfxID, double theBase) = 0;
    virtual int SetBasePan(int theSfxID, int thePan) = 0;
    virtual int SetBasePitch(int theSfxID, float thePitch) = 0;
    virtual bool IsPlaying(int theSfxID) = 0;
    virtual SoundInstance *GetPlayingSoundInstance(int theSfxID) = 0;
    virtual void ReleaseChannels() = 0;
    virtual void ReleaseFreeChannels() = 0;
    virtual bool Initialized() = 0;
    virtual int FindFreeChannel() = 0;
};

} // namespace Sexy

#endif // PVZ_SEXYAPPFRAMEWORK_SOUND_SOUND_MANAGER_H
