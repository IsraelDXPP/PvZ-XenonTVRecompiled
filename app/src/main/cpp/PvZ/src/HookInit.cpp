#include "PvZ/HookInit.h"
#include "PvZ/Symbols.h"
#include "PvZ/Lawn/Widget/TitleScreen.h"
#include "PvZ/TodLib/Common/TodFoley.h"
#include "PvZ/SexyAppFramework/Sound/AudiereSoundDriver.h"
#include "PvZ/SexyAppFramework/Sound/AudiereSoundManager.h"
#include "Homura/HookUtils.h"
#include "Homura/Logger.h"
#include "Homura/MemoryUtils.h"

// PlaySample function offsets in libGameMain.so
static constexpr uintptr_t PLAY_SAMPLE_OFFSET = 0x398BBC;
static constexpr uintptr_t PLAY_SAMPLE_VOL_OFFSET = 0x398BFC;
static constexpr uintptr_t PLAY_SAMPLE_SINGLE_OFFSET = 0x398C54;
static constexpr uintptr_t PLAY_SAMPLE_SINGLE_VOL_OFFSET = 0x398CD0;

// Old function pointers
static int (*old_PlaySample)(int, int);
static int (*old_PlaySample_Vol)(int, int, int);
static int (*old_PlaySampleSingle)(int, int);
static int (*old_PlaySampleSingle_Vol)(int, int, int);

// Ensure sound is loaded in the binary first, then import to our system
static void EnsureSoundImported(Sexy::AudiereSoundManager *mgr, int this_, int soundId) {
    if (!mgr || soundId < 0 || soundId >= 256) return;
    if (mgr->mSoundInfo[soundId].mSource) return;

    // Binary's PlaySample loads the sound internally via LoadSound.
    // The binary's audio backend may be broken/uninitialized on Android,
    // so this may produce no audible output on its own. We just need the
    // sound to be loaded into the binary's arrays.
    old_PlaySample(this_, soundId);

    mgr->ImportSoundFromBinary(soundId);
}

// PlaySample(int this, int soundId)
static int PlaySample_Hook(int this_, int a2) {
    auto *mgr = Sexy::GetAudiereSoundManager();
    if (!mgr) return old_PlaySample(this_, a2);
    EnsureSoundImported(mgr, this_, a2);
    auto *inst = mgr->GetSoundInstance(a2);
    if (inst) inst->Play(false, true);
    return reinterpret_cast<int>(inst);
}

// PlaySample(int this, int soundId, int volumeOrFlag)
static int PlaySample_Vol_Hook(int this_, int a2, int a3) {
    auto *mgr = Sexy::GetAudiereSoundManager();
    if (!mgr) return old_PlaySample_Vol(this_, a2, a3);
    EnsureSoundImported(mgr, this_, a2);
    auto *inst = mgr->GetSoundInstance(a2);
    if (inst) {
        inst->Play(false, true);
    }
    return reinterpret_cast<int>(inst);
}

// PlaySampleSingle(int this, int soundId)
static int PlaySampleSingle_Hook(int this_, int a2) {
    auto *mgr = Sexy::GetAudiereSoundManager();
    if (!mgr) return old_PlaySampleSingle(this_, a2);
    EnsureSoundImported(mgr, this_, a2);
    auto *inst = mgr->GetSoundInstance(a2);
    if (inst) inst->Play(false, true);
    return reinterpret_cast<int>(inst);
}

// PlaySampleSingle(int this, int soundId, int volumeOrFlag)
static int PlaySampleSingle_Vol_Hook(int this_, int a2, int a3) {
    auto *mgr = Sexy::GetAudiereSoundManager();
    if (!mgr) return old_PlaySampleSingle_Vol(this_, a2, a3);
    EnsureSoundImported(mgr, this_, a2);
    auto *inst = mgr->GetSoundInstance(a2);
    if (inst) {
        inst->Play(false, true);
    }
    return reinterpret_cast<int>(inst);
}

void InitHookFunction() {
    LOG_INFO("Hooking TitleScreen functions");
    homura::HookFunction(
        TitleScreen_DrawAddr,
        TitleScreen_Draw,
        &old_TitleScreen_Draw
    );
    homura::HookFunction(
        TitleScreen_UpdateAddr,
        TitleScreen_Update,
        &old_TitleScreen_Update
    );
    homura::HookFunction(
        TitleScreen_SwitchStateAddr,
        TitleScreen_SwitchState,
        &old_TitleScreen_SwitchState
    );

    LOG_INFO("Hooking PlaySample functions");
    {
        uintptr_t base = homura::GetLibBaseAddr("libGameMain.so");
        if (base) {
            homura::HookFunction(
                reinterpret_cast<void *>(base + PLAY_SAMPLE_OFFSET),
                PlaySample_Hook,
                &old_PlaySample
            );
            homura::HookFunction(
                reinterpret_cast<void *>(base + PLAY_SAMPLE_VOL_OFFSET),
                PlaySample_Vol_Hook,
                &old_PlaySample_Vol
            );
            homura::HookFunction(
                reinterpret_cast<void *>(base + PLAY_SAMPLE_SINGLE_OFFSET),
                PlaySampleSingle_Hook,
                &old_PlaySampleSingle
            );
            homura::HookFunction(
                reinterpret_cast<void *>(base + PLAY_SAMPLE_SINGLE_VOL_OFFSET),
                PlaySampleSingle_Vol_Hook,
                &old_PlaySampleSingle_Vol
            );
        } else {
            LOG_ERROR("Failed to get libGameMain.so base address");
        }
    }

    LOG_INFO("Hooking TodFoley functions");
    homura::HookFunction<FoleyInstance*(TodFoley*, FoleyType)>(
        TodFoley_PlayFoleyAddr,
        [](TodFoley *t, FoleyType type) { return t->PlayFoley(type); },
        nullptr
    );
    homura::HookFunction<FoleyInstance*(TodFoley*, FoleyType, float)>(
        TodFoley_PlayFoleyPitchAddr,
        [](TodFoley *t, FoleyType type, float p) { return t->PlayFoleyPitch(type, p); },
        nullptr
    );

    homura::HookFunction<bool(TodFoley*, FoleyType)>(
        TodFoley_IsFoleyPlayingAddr,
        [](TodFoley *t, FoleyType type) { return t->IsFoleyPlaying(type); },
        nullptr
    );
    homura::HookFunction<void(TodFoley*, FoleyType)>(
        TodFoley_StopFoleyAddr,
        [](TodFoley *t, FoleyType type) { t->StopFoley(type); },
        nullptr
    );
    homura::HookFunction<void(TodFoley*)>(
        TodFoley_CancelPausedFoleyAddr,
        [](TodFoley *t) { t->CancelPausedFoley(); },
        nullptr
    );
    homura::HookFunction<void(TodFoley*, bool)>(
        TodFoley_GamePauseAddr,
        [](TodFoley *t, bool p) { t->GamePause(p); },
        nullptr
    );
    homura::HookFunction<void(TodFoley*)>(
        TodFoley_RehookupSoundWithMusicVolumeAddr,
        [](TodFoley *t) { t->RehookupSoundWithMusicVolume(); },
        nullptr
    );
    homura::HookFunction<void(FoleyParams*, int)>(
        TodFoleyInitializeAddr,
        TodFoleyInitialize,
        nullptr
    );

}

void InitVTableHookFunction() {}

void InitOpenSL() {
    LOG_INFO("Initializing decompiled audio system");
    Sexy::AudiereSoundDriver::Init();
}

void InitIntroVideo() {}
