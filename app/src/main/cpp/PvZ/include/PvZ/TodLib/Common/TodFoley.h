/*
 * Copyright (C) 2023-2026  PvZ TV Touch Team
 *
 * This file is part of PlantsVsZombies-AndroidTV.
 *
 * PlantsVsZombies-AndroidTV is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * PlantsVsZombies-AndroidTV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * PlantsVsZombies-AndroidTV.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PVZ_SEXYAPPFRAMEWORK_TODLIB_COMMON_TOD_FOLEY_H
#define PVZ_SEXYAPPFRAMEWORK_TODLIB_COMMON_TOD_FOLEY_H

#include "PvZ/Lawn/Common/ConstEnums.h"
#include "PvZ/Lawn/Common/Resources.h"
#include "PvZ/SexyAppFramework/Sound/SoundInstance.h"
#include "PvZ/SexyAppFramework/Sound/SoundManager.h"


inline constexpr int MAX_FOLEY_TYPES = 110;
inline constexpr int MAX_FOLEY_INSTANCES = 8;

// ######################################################################################################################################################
// ############################################################# 以下为拟音音效定义相关内容 #############################################################
// ######################################################################################################################################################

class FoleyParams {
public:
    FoleyType mFoleyType;
    float mPitchRange;
    int *mSfxID[10];
    unsigned int mFoleyFlags;
};

FoleyParams *LookupFoley(FoleyType theFoleyType);
void TodFoleyInitialize(FoleyParams *theFoleyParamArray, int theFoleyParamArraySize);

auto GetNewLawnFoleyParamArray() -> FoleyParams (&)[FoleyType::EXTENDED_NUM_FOLEY];

// ######################################################################################################################################################
// ############################################################ 以下正式开始拟音音效相关声明 ############################################################
// ######################################################################################################################################################

class FoleyInstance {
public:
    Sexy::SoundInstance *mInstance;
    int mRefCount;
    bool _paused;
    int mStartTime;
    int mPauseOffset;

    FoleyInstance();
};

struct FoleyTypeData {
    FoleyInstance mFoleyInstances[MAX_FOLEY_INSTANCES];
    int mLastVariationPlayed;

    FoleyTypeData();
};

class TodFoley {
public:
    FoleyTypeData mTypeData[MAX_FOLEY_TYPES];

    bool IsFoleyPlaying(FoleyType theFoleyType);
    void StopFoley(FoleyType theFoleyType);
    void CancelPausedFoley();
    void GamePause(bool thePause);
    void RehookupSoundWithMusicVolume();
    FoleyInstance *PlayFoley(FoleyType theFoleyType);
    FoleyInstance *PlayFoleyPitch(FoleyType theFoleyType, float thePitch);
    void ApplyMusicVolume(FoleyInstance *theFoleyInstance);
};

// SoundSystemFindInstance and helpers: reimplemented in TodFoley.cpp
FoleyInstance *SoundSystemFindInstance(TodFoley *theSoundSystem, FoleyType theFoleyType);
FoleyInstance *SoundSystemGetFreeInstanceIndex(TodFoley *theSoundSystem, FoleyType theFoleyType);
bool SoundSystemHasFoleyPlayedTooRecently(TodFoley *theSoundSystem, FoleyType theFoleyType);
void SoundSystemReleaseFinishedInstances(TodFoley *theSoundSystem);
void TodFoleyDispose();

#endif // PVZ_SEXYAPPFRAMEWORK_TODLIB_COMMON_TOD_FOLEY_H
