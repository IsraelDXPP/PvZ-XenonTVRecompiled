#ifndef PVZ_SEXYAPPFRAMEWORK_SOUND_AUDIERE_SOUND_MANAGER_H
#define PVZ_SEXYAPPFRAMEWORK_SOUND_AUDIERE_SOUND_MANAGER_H

#include "PvZ/STL/pvzstl_string.h"
#include "SoundManager.h"
#include "OutputStream.h"
#include "audiere/AudioTypes.h"

namespace Sexy {

class AudiereSoundInstance;

struct AudiereSoundInfo {
    audiere::SampleSource *mSource;
    int mDataSize;
    int mSampleRate;
    int mChannels;
    int mBitsPerSample;

    AudiereSoundInfo();
};

class AudiereSoundManager : public SoundManager {
public:
    AudiereSoundManager();
    ~AudiereSoundManager() override;

    int LoadSound(const pvzstl::string &theFileName) override;
    int LoadSound(int theSfxID, const pvzstl::string &theFileName) override;
    int GetFreeSoundId() override;
    int GetNumSounds() override;
    void StopAllSounds() override;
    SoundInstance *GetSoundInstance(int theSfxID) override;
    void SetVolume(double theVolume) override;
    void SetMasterVolume(double theVolume) override;
    double GetMasterVolume() override;
    int SetBaseVolume(int theSfxID, double theBase) override;
    int SetBasePan(int theSfxID, int thePan) override;
    int SetBasePitch(int theSfxID, float thePitch) override;
    bool IsPlaying(int theSfxID) override;
    SoundInstance *GetPlayingSoundInstance(int theSfxID) override;
    void ReleaseChannels() override;
    void ReleaseFreeChannels() override;
    bool Initialized() override;
    int FindFreeChannel() override;

    float GetMasterVolumeFloat() const { return mMasterVolume; }
    audiere::AndroidAudioDevice *GetAudioDevice() const { return mAudioDevice; }
    void ImportFromBinary();
    void ImportSoundFromBinary(int slot);

    static const int MAX_SOUNDS = 256;
    static const int MAX_CHANNELS = 32;

    AudiereSoundInfo mSoundInfo[MAX_SOUNDS];
    float mBaseVolume[MAX_SOUNDS];
    int mBasePan[MAX_SOUNDS];
    float mBasePitch[MAX_SOUNDS];
    AudiereSoundInstance *mChannels[MAX_CHANNELS];
    int mChannelIds[MAX_CHANNELS];
    float mMasterVolume;
    audiere::AndroidAudioDevice *mAudioDevice;
};

} // namespace Sexy

#endif // PVZ_SEXYAPPFRAMEWORK_SOUND_AUDIERE_SOUND_MANAGER_H
