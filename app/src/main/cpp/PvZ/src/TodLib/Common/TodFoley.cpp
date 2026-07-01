#include "PvZ/TodLib/Common/TodFoley.h"
#include "PvZ/SexyAppFramework/Misc/Common.h"
#include "PvZ/Lawn/LawnApp.h"
#include "PvZ/TodLib/Common/TodCommon.h"
#include "PvZ/SexyAppFramework/Sound/AudiereSoundDriver.h"
#include "PvZ/SexyAppFramework/Sound/AudiereSoundManager.h"
#include "PvZ/Symbols.h"
#include <algorithm>

// Read values from binary's SexyAppBase instance
static int getUpdateCount() {
    return *reinterpret_cast<int *>(reinterpret_cast<char *>(Sexy::gSexyAppBase) + 1104);
}

static double getSfxVolume() {
    return *reinterpret_cast<double *>(reinterpret_cast<char *>(Sexy::gSexyAppBase) + 80);
}

static double getMusicVolume() {
    return *reinterpret_cast<double *>(reinterpret_cast<char *>(Sexy::gSexyAppBase) + 88);
}

void TodFoleyDispose() {
    gFoleyParamArray = nullptr;
    gFoleyParamArraySize = 0;
}

FoleyInstance::FoleyInstance() {
    mInstance = nullptr;
    mRefCount = 0;
    _paused = false;
    mStartTime = 0;
    mPauseOffset = 0;
}

FoleyTypeData::FoleyTypeData() {
    for (int i = 0; i < MAX_FOLEY_INSTANCES; i++) {
        mFoleyInstances[i] = FoleyInstance();
    }
    mLastVariationPlayed = -1;
}

FoleyInstance *SoundSystemFindInstance(TodFoley *theSoundSystem, FoleyType theFoleyType) {
    int offset = 0;
    int indexMultiplier = 164 * theFoleyType;
    int loopCount = 0;
    while (true) {
        FoleyInstance *anInstance = reinterpret_cast<FoleyInstance *>(reinterpret_cast<char *>(theSoundSystem) + indexMultiplier + offset);
        int valIdx = 5 * loopCount;
        offset += 20;
        loopCount++;
        FoleyInstance *result = reinterpret_cast<FoleyInstance *>(reinterpret_cast<char *>(theSoundSystem) + indexMultiplier + 4 * valIdx);
        if (anInstance->mRefCount > 0) {
            return result;
        }
        if (loopCount == 8) {
            return nullptr;
        }
    }
}

bool SoundSystemHasFoleyPlayedTooRecently(TodFoley *theSoundSystem, FoleyType theFoleyType) {
    int loopCount = 0;
    for (FoleyInstance *anInstance = &theSoundSystem->mTypeData[theFoleyType].mFoleyInstances[0]; ; anInstance++) {
        loopCount++;
        if (anInstance->mInstance != nullptr) {
            if (getUpdateCount() - anInstance->mStartTime <= 9) {
                return true;
            }
        }
        if (loopCount == 8) {
            return false;
        }
    }
}

FoleyInstance *SoundSystemGetFreeInstanceIndex(TodFoley *theSoundSystem, FoleyType theFoleyType) {
    int offset = 0;
    int indexMultiplier = 164 * theFoleyType;
    int loopCount = 0;
    while (true) {
        FoleyInstance *anInstance = reinterpret_cast<FoleyInstance *>(reinterpret_cast<char *>(theSoundSystem) + indexMultiplier + offset);
        int valIdx = 5 * loopCount;
        offset += 20;
        loopCount++;
        FoleyInstance *result = reinterpret_cast<FoleyInstance *>(reinterpret_cast<char *>(theSoundSystem) + indexMultiplier + 4 * valIdx);
        if (anInstance->mInstance == nullptr) {
            return result;
        }
        if (loopCount == 8) {
            return nullptr;
        }
    }
}

void TodFoley::ApplyMusicVolume(FoleyInstance *theFoleyInstance) {
    if (!theFoleyInstance->mInstance) return;
    Sexy::SoundInstance *instance = theFoleyInstance->mInstance;
    double musicVol = getMusicVolume();
    if (musicVol < 0.000000999999997) {
        instance->SetVolume(0.0);
    } else {
        instance->SetVolume(getSfxVolume() / musicVol);
    }
}

void SoundSystemReleaseFinishedInstances(TodFoley *theSoundSystem) {
    if (gFoleyParamArraySize > 0) {
        for (int i = 0; i < gFoleyParamArraySize; i++) {
            int j = 0;
            FoleyInstance *anInstance = &theSoundSystem->mTypeData[i].mFoleyInstances[0];
            do {
                while (true) {
                    if (anInstance->mRefCount == 0) {
                        gInAssert = 1;
                        if (anInstance->mInstance != nullptr) {
                            _assert2("games/pvz/main/Lawn/TodLib/TodFoley.cpp", 69, "void SoundSystemReleaseFinishedInstances(TodFoley*)", "aFoleyInstance->mInstance == __null");
                        }
                        gInAssert = 0;
                        j++;
                        anInstance++;
                        if (j == 8) {
                            goto next_foley_type;
                        }
                        continue;
                    }

                    if (!anInstance->_paused) {
                        Sexy::SoundInstance *instancePtr = anInstance->mInstance;
                        gInAssert = 1;
                        if (instancePtr == nullptr) {
                            _assert2("games/pvz/main/Lawn/TodLib/TodFoley.cpp", 78, "void SoundSystemReleaseFinishedInstances(TodFoley*)", "aFoleyInstance->mInstance");
                            instancePtr = anInstance->mInstance;
                        }
                        gInAssert = 0;
                        if (!instancePtr->IsPlaying()) {
                            break;
                        }
                    }
                    j++;
                    anInstance++;
                    if (j == 8) {
                        goto next_foley_type;
                    }
                }
                j++;
                anInstance->mInstance->Release();
                anInstance->mRefCount = 0;
                anInstance->mInstance = nullptr;
                anInstance++;
            } while (j != 8);
        next_foley_type:;
        }
    }
}

bool TodFoley::IsFoleyPlaying(FoleyType theFoleyType) {
    SoundSystemReleaseFinishedInstances(this);
    return SoundSystemFindInstance(this, theFoleyType) != nullptr;
}

void TodFoley::CancelPausedFoley() {
    SoundSystemReleaseFinishedInstances(this);
    if (gFoleyParamArraySize > 0) {
        int variationIndex = 0;
        do {
            int currentInstanceIdx = 0;
            FoleyInstance *anInstance = &this->mTypeData[variationIndex].mFoleyInstances[0];
            do {
                while (true) {
                    currentInstanceIdx++;
                    if (anInstance->mRefCount != 0) {
                        if (anInstance->_paused) {
                            break;
                        }
                    }
                    anInstance++;
                    if (currentInstanceIdx == 8) {
                        goto next_variation;
                    }
                }
                Sexy::SoundInstance *instance = anInstance->mInstance;
                anInstance->mRefCount = 0;
                instance->Release();
                anInstance->mInstance = nullptr;
                anInstance++;
            } while (currentInstanceIdx != 8);
        next_variation:
            variationIndex++;
        } while (gFoleyParamArraySize > variationIndex);
    }
}

void TodFoley::StopFoley(FoleyType theFoleyType) {
    SoundSystemReleaseFinishedInstances(this);
    FoleyInstance *anInstance = SoundSystemFindInstance(this, theFoleyType);
    if (anInstance != nullptr) {
        int refCount = anInstance->mRefCount;
        gInAssert = 1;
        if (refCount <= 0) {
            _assert2("games/pvz/main/Lawn/TodLib/TodFoley.cpp", 260, "void TodFoley::StopFoley(FoleyType)", "aFoleyInstance->mRefCount > 0");
        }
        if (anInstance->mInstance == nullptr) {
            _assert2("games/pvz/main/Lawn/TodLib/TodFoley.cpp", 261, "void TodFoley::StopFoley(FoleyType)", "aFoleyInstance->mInstance");
        }
        int newRefCount = refCount - 1;
        anInstance->mRefCount = newRefCount;
        gInAssert = 0;
        if (refCount == 1) {
            anInstance->mInstance->Release();
            anInstance->mInstance = nullptr;
        }
    }
}

FoleyParams *LookupFoley(FoleyType theFoleyType) {
    gInAssert = 1;
    if (theFoleyType < 0 || theFoleyType >= gFoleyParamArraySize) {
        _assert2("games/pvz/main/Lawn/TodLib/TodFoley.cpp", 48, "FoleyParams* LookupFoley(FoleyType)", "theFoleyType >= 0 && theFoleyType < gFoleyParamArraySize");
        if (gFoleyParamArraySize <= 109) {
            goto label_4;
        }
    } else {
        if (gFoleyParamArraySize <= 109) {
            goto label_4;
        }
    }
    _assert2("games/pvz/main/Lawn/TodLib/TodFoley.cpp", 49, "FoleyParams* LookupFoley(FoleyType)", "gFoleyParamArraySize < MAX_FOLEY_TYPES");
label_4:
    FoleyParams *aFoleyParams = &gFoleyParamArray[theFoleyType];
    if (aFoleyParams->mFoleyType != theFoleyType) {
        _assert2("games/pvz/main/Lawn/TodLib/TodFoley.cpp", 53, "FoleyParams* LookupFoley(FoleyType)", "aFoleyParams->mFoleyType == theFoleyType");
    }
    gInAssert = 0;
    return aFoleyParams;
}

void TodFoley::RehookupSoundWithMusicVolume() {
    SoundSystemReleaseFinishedInstances(this);
    if (gFoleyParamArraySize > 0) {
        for (int i = 0; i < gFoleyParamArraySize; i++) {
            FoleyParams *aFoleyParams = LookupFoley(static_cast<FoleyType>(i));
            if ((aFoleyParams->mFoleyFlags & 8) != 0) {
                int j = 0;
                FoleyInstance *anInstance = &this->mTypeData[i].mFoleyInstances[0];
                do {
                    while (true) {
                        if (anInstance->mRefCount != 0) {
                            break;
                        }
                        j++;
                        anInstance++;
                        if (j == 8) {
                            goto next_foley;
                        }
                    }
                    ApplyMusicVolume(anInstance);
                    j++;
                    anInstance++;
                } while (j != 8);
            }
        next_foley:;
        }
    }
}

void TodFoley::GamePause(bool thePause) {
    SoundSystemReleaseFinishedInstances(this);
    if (gFoleyParamArraySize > 0) {
        for (int i = 0; i < gFoleyParamArraySize; i++) {
            FoleyParams *aFoleyParams = LookupFoley(static_cast<FoleyType>(i));
            if ((aFoleyParams->mFoleyFlags & 4) == 0) {
                int j = 0;
                FoleyInstance *anInstance = &this->mTypeData[i].mFoleyInstances[0];
                do {
                    while (true) {
                        if (anInstance->mRefCount != 0) {
                            break;
                        }
                        j++;
                        anInstance++;
                        if (j == 8) {
                            goto next_foley_type;
                        }
                    }
                    if (!thePause) {
                        if (anInstance->_paused) {
                            anInstance->_paused = false;
                            anInstance->mInstance->Play((aFoleyParams->mFoleyFlags & 1) != 0, false);
                        }
                    } else {
                        Sexy::SoundInstance *instance = anInstance->mInstance;
                        anInstance->mPauseOffset = 0;
                        anInstance->_paused = true;
                        instance->Stop();
                    }
                    j++;
                    anInstance++;
                } while (j != 8);
            }
        next_foley_type:;
        }
    }
}

void TodFoleyInitialize(FoleyParams *theFoleyParamArray, int theFoleyParamArraySize) {
    // Import sounds from the binary's SM into ours (after binary has loaded them)
    Sexy::AudiereSoundManager *ourSM = Sexy::GetAudiereSoundManager();
    if (ourSM) {
        ourSM->ImportFromBinary();
    }

    gInAssert = 1;
    if (gFoleyParamArray != nullptr || gFoleyParamArraySize != 0) {
        _assert2("games/pvz/main/Lawn/TodLib/TodFoley.cpp", 20, "void TodFoleyInitialize(FoleyParams*, int)", "gFoleyParamArray == __null && gFoleyParamArraySize == 0");
    }

    gFoleyParamArraySize = theFoleyParamArraySize;
    gInAssert = 0;
    gFoleyParamArray = theFoleyParamArray;
}

static int TodPickFromArray(const int *theArray, int theCount) {
    gInAssert = 1;
    if (theCount <= 0) {
        _assert2("games/pvz/main/Lawn/TodLib/TodFoley.cpp", 145, "int TodPickFromArray(const int*, int)", "theCount > 0");
    }
    gInAssert = 0;
    return theArray[Sexy::Rand(theCount)];
}

FoleyInstance *TodFoley::PlayFoleyPitch(FoleyType theFoleyType, float thePitch) {
    FoleyParams *aFoleyParams = LookupFoley(theFoleyType);
    SoundSystemReleaseFinishedInstances(this);
    bool tooRecently = SoundSystemHasFoleyPlayedTooRecently(this, theFoleyType);
    if (tooRecently) {
        if ((aFoleyParams->mFoleyFlags & 1) == 0) {
            return reinterpret_cast<FoleyInstance *>(tooRecently);
        }
    }
    if ((aFoleyParams->mFoleyFlags & 2) != 0) {
        FoleyInstance *anInstance = SoundSystemFindInstance(this, theFoleyType);
        if (anInstance != nullptr) {
            anInstance->mRefCount++;
            anInstance->mStartTime = getUpdateCount();
            return anInstance;
        }
    }

    FoleyInstance *anInstance = SoundSystemGetFreeInstanceIndex(this, theFoleyType);
    if (anInstance != nullptr) {
        int aVariations = 0;
        int aVariationArray[10];
        bool dontRepeat = (aFoleyParams->mFoleyFlags & 0x10) != 0;
        int lastVariation = this->mTypeData[theFoleyType].mLastVariationPlayed;
        for (int i = 0; i < 10; i++) {
            if (!dontRepeat || lastVariation != i) {
                if (aFoleyParams->mSfxID[i] == nullptr) {
                    break;
                }
                aVariationArray[aVariations++] = i;
            }
        }
        gInAssert = 1;
        if (aVariations <= 0) {
            _assert2("games/pvz/main/Lawn/TodLib/TodFoley.cpp", 201, "void TodFoley::PlayFoleyPitch(FoleyType, float)", "aVariations > 0");
        }
        gInAssert = 0;
        int variation = TodPickFromArray(aVariationArray, aVariations);
        this->mTypeData[theFoleyType].mLastVariationPlayed = variation;

        Sexy::SoundManager *soundManager = Sexy::GetAudiereSoundManager();
        int sfxID = *aFoleyParams->mSfxID[variation];
        Sexy::SoundInstance *soundInstance = soundManager ? soundManager->GetSoundInstance(sfxID) : nullptr;
        if (soundInstance != nullptr) {
            anInstance->mRefCount = 1;
            anInstance->mInstance = soundInstance;
            anInstance->mStartTime = getUpdateCount();
            this->mTypeData[theFoleyType].mLastVariationPlayed = variation;
            if (thePitch != 0.0) {
                soundInstance->AdjustBasePitch(thePitch);
            }
            if ((aFoleyParams->mFoleyFlags & 8) != 0) {
                ApplyMusicVolume(anInstance);
            }
            soundInstance->Play(false, false);
            return anInstance;
        }
    }
    return nullptr;
}

FoleyInstance *TodFoley::PlayFoley(FoleyType theFoleyType) {
    FoleyParams *aFoleyParams = LookupFoley(theFoleyType);
    float pitchRange = aFoleyParams->mPitchRange;
    float aPitch = 0.0f;
    if (pitchRange != 0.0f) {
        float randVal = Sexy::Rand(pitchRange * 2.0f);
        aPitch = 1.0f + randVal - pitchRange;
    }
    return PlayFoleyPitch(theFoleyType, aPitch);
}
