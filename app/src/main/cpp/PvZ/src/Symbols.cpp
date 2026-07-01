#include "PvZ/Symbols.h"
#include "PvZ/TodLib/Common/TodFoley.h"
#include "Homura/DynamicLibUtils.h"
#include "Homura/Logger.h"

[[gnu::init_priority(101)]] static homura::SharedObjLoader libGameMain{"libGameMain.so"};

bool LoadGameMain() {
    if (!libGameMain.IsOpen()) {
        return false;
    }
    return true;
}

Sexy::Image *GetTitleScreenImage() {
    static Sexy::Image *image = nullptr;
    if (!image) {
        auto addr = libGameMain.GetSymbol("_ZN4Sexy17IMAGE_TITLESCREENE");
        if (addr) image = *reinterpret_cast<Sexy::Image **>(addr);
    }
    return image;
}

Sexy::Image *GetImageLoadBarDirt() {
    static Sexy::Image *image = nullptr;
    if (!image) {
        auto addr = libGameMain.GetSymbol("_ZN4Sexy18IMAGE_LOADBAR_DIRTE");
        if (addr) image = *reinterpret_cast<Sexy::Image **>(addr);
    }
    return image;
}

Sexy::Image *GetImageLoadBarGrass() {
    static Sexy::Image *image = nullptr;
    if (!image) {
        auto addr = libGameMain.GetSymbol("_ZN4Sexy19IMAGE_LOADBAR_GRASSE");
        if (addr) image = *reinterpret_cast<Sexy::Image **>(addr);
    }
    return image;
}

Sexy::Image *GetImageTitleScreenBall() {
    static Sexy::Image *image = nullptr;
    if (!image) {
        auto addr = libGameMain.GetSymbol("_ZN4Sexy22IMAGE_TITLESCREEN_BALLE");
        if (addr) image = *reinterpret_cast<Sexy::Image **>(addr);
    }
    return image;
}

Sexy::Image *GetImageTitleScreenGlow() {
    static Sexy::Image *image = nullptr;
    if (!image) {
        auto addr = libGameMain.GetSymbol("_ZN4Sexy22IMAGE_TITLESCREEN_GLOWE");
        if (addr) image = *reinterpret_cast<Sexy::Image **>(addr);
    }
    return image;
}

Sexy::Font *GetFontHouseOfTerror20() {
    static Sexy::Font *font = nullptr;
    if (!font) {
        auto addr = libGameMain.GetSymbol("_ZN4Sexy20FONT_HOUSEOFTERROR20E");
        if (addr) font = *reinterpret_cast<Sexy::Font **>(addr);
    }
    return font;
}

Sexy::Image *GetImagePvzLogo() {
    static Sexy::Image *image = nullptr;
    if (!image) {
        auto addr = libGameMain.GetSymbol("_ZN4Sexy14IMAGE_PVZ_LOGOE");
        if (addr) image = *reinterpret_cast<Sexy::Image **>(addr);
    }
    return image;
}

Sexy::Image *GetImageReanimSodrollCap() {
    static Sexy::Image *image = nullptr;
    if (!image) {
        auto addr = libGameMain.GetSymbol("_ZN4Sexy23IMAGE_REANIM_SODROLLCAPE");
        if (addr) image = *reinterpret_cast<Sexy::Image **>(addr);
    }
    return image;
}

Sexy::Image *GetImageEsrbRating() {
    static Sexy::Image *image = nullptr;
    if (!image) {
        auto addr = libGameMain.GetSymbol("_ZN4Sexy17IMAGE_ESRB_RATINGE");
        if (addr) image = *reinterpret_cast<Sexy::Image **>(addr);
    }
    return image;
}

Sexy::Font *GetFontDwarvenTodcraft24() {
    static Sexy::Font *font = nullptr;
    if (!font) {
        auto addr = libGameMain.GetSymbol("_ZN4Sexy22FONT_DWARVENTODCRAFT24E");
        if (addr) font = *reinterpret_cast<Sexy::Font **>(addr);
    }
    return font;
}

bool LoadNativeCode() {
    TitleScreen_DrawAddr = libGameMain.GetSymbol("_ZN11TitleScreen4DrawEPN4Sexy8GraphicsE");
    TitleScreen_UpdateAddr = libGameMain.GetSymbol("_ZN11TitleScreen6UpdateEv");
    TitleScreen_SwitchStateAddr = libGameMain.GetSymbol("_ZN11TitleScreen11SwitchStateE10TitleStatei");
    TodStringTranslateAddr = libGameMain.GetSymbol("_Z18TodStringTranslatePKc");
    TodDrawStringAddr = libGameMain.GetSymbol("_Z13TodDrawStringPN4Sexy8GraphicsERKSsiiPNS_4FontENS_5ColorE23DrawStringJustification");
    Sexy_Graphics_DrawStringAddr = libGameMain.GetSymbol("_ZN4Sexy8Graphics10DrawStringERKSsii");
    Sexy_Graphics_SetFontAddr = libGameMain.GetSymbol("_ZN4Sexy8Graphics7SetFontEPNS_4FontE");
    Sexy_Graphics_SetColorAddr = libGameMain.GetSymbol("_ZN4Sexy8Graphics8SetColorERKNS_5ColorE");
    Sexy_Graphics_FillRectAddr = libGameMain.GetSymbol("_ZN4Sexy8Graphics8FillRectERKNS_5TRectIiEE");
    Sexy_Graphics_DrawImageAddr = libGameMain.GetSymbol("_ZN4Sexy8Graphics9DrawImageEPNS_5ImageEii");
    Sexy_Graphics_SetLinearBlendAddr = libGameMain.GetSymbol("_ZN4Sexy8Graphics14SetLinearBlendEb");
    Sexy_Widget_DrawAddr = libGameMain.GetSymbol("_ZN4Sexy6Widget4DrawEPNS_8GraphicsE");
    Reanimation_DrawAddr = libGameMain.GetSymbol("_ZN11Reanimation4DrawEPN4Sexy8GraphicsE");
    Sexy_GetTickCountAddr = libGameMain.GetSymbol("_ZN4Sexy12GetTickCountEv");
    if (!Sexy_GetTickCountAddr) {
        Sexy_GetTickCountAddr = libGameMain.GetSymbol("_Z12GetTickCountv");
    }
    if (!Sexy_GetTickCountAddr) {
        Sexy_GetTickCountAddr = libGameMain.GetSymbol("GetTickCount");
    }
    Sexy_Graphics_SetColorizeImagesAddr = libGameMain.GetSymbol("_ZN4Sexy8Graphics17SetColorizeImagesEb");
    Sexy_Widget_DeferOverlayAddr = libGameMain.GetSymbol("_ZN4Sexy6Widget12DeferOverlayEi");
    Sexy_Graphics_GraphicsAddr = libGameMain.GetSymbol("_ZN4Sexy8GraphicsC1ERKS0_");
    if (!Sexy_Graphics_GraphicsAddr) {
        Sexy_Graphics_GraphicsAddr = libGameMain.GetSymbol("_ZN4Sexy8GraphicsC2ERKS0_");
    }
    Sexy_Graphics_DeleteAddr = libGameMain.GetSymbol("_ZN4Sexy8GraphicsD1Ev");
    if (!Sexy_Graphics_DeleteAddr) {
        Sexy_Graphics_DeleteAddr = libGameMain.GetSymbol("_ZN4Sexy8GraphicsD2Ev");
    }
    Sexy_Graphics_ClipRectAddr = libGameMain.GetSymbol("_ZN4Sexy8Graphics8ClipRectEiiii");
    Sexy_Graphics_DrawImage3Addr = libGameMain.GetSymbol("_ZN4Sexy8Graphics9DrawImageEPNS_5ImageEiiii");
    Sexy_Graphics_DrawImageMirror2Addr = libGameMain.GetSymbol("_ZN4Sexy8Graphics15DrawImageMirrorEPNS_5ImageERKNS_5TRectIiEES6_b");
    TodAnimateCurveAddr = libGameMain.GetSymbol("_Z15TodAnimateCurveiiiii9TodCurves");
    TodAnimateCurveFloatTimeAddr = libGameMain.GetSymbol("_Z24TodAnimateCurveFloatTimefffff9TodCurves");
    TodScaleRotateTransformMatrixAddr = libGameMain.GetSymbol("_Z29TodScaleRotateTransformMatrixRN4Sexy11SexyMatrix3Efffff");
    TodBltMatrixAddr = libGameMain.GetSymbol("_Z12TodBltMatrixPN4Sexy8GraphicsEPNS_5ImageERKNS_11SexyMatrix3ERKNS_5TRectIiEERKNS_5ColorEiSA_");
    
    Sexy_Widget_UpdateAddr = libGameMain.GetSymbol("_ZN4Sexy6Widget6UpdateEv");
    Sexy_SexyAppBase_StartLoadingThreadAddr = libGameMain.GetSymbol("_ZN4Sexy11SexyAppBase18StartLoadingThreadEv");
    Sexy_GetEnvOptionAddr = libGameMain.GetSymbol("_ZN4Sexy12GetEnvOptionEPKcS1_b");
    EffectSystem_UpdateAddr = libGameMain.GetSymbol("_ZN12EffectSystem6UpdateEv");
    TypingCheck_SetPhraseAddr = libGameMain.GetSymbol("_ZN11TypingCheck9SetPhraseERKSs");
    
    AGViewAddEventListenerAddr = libGameMain.GetSymbol("AGViewAddEventListener");
    AGViewRemoveEventListenerAddr = libGameMain.GetSymbol("AGViewRemoveEventListener");
    AGViewShowAddr = libGameMain.GetSymbol("AGViewShow");
    AGGetModelAddr = libGameMain.GetSymbol("AGGetModel");
    AGGetSdkVersionAddr = libGameMain.GetSymbol("AGGetSdkVersion");
    AGVideoPlayAddr = libGameMain.GetSymbol("AGVideoPlay");
    AGVideoOpenAddr = libGameMain.GetSymbol("AGVideoOpen");
    AGVideoShowAddr = libGameMain.GetSymbol("AGVideoShow");
    AGVideoIsPlayingAddr = libGameMain.GetSymbol("AGVideoIsPlaying");
    AGVideoCloseAddr = libGameMain.GetSymbol("AGVideoClose");

    LawnApp_AddReanimationAddr = libGameMain.GetSymbol("_ZN7LawnApp14AddReanimationEffi15ReanimationType");
    Reanimation_OverrideScaleAddr = libGameMain.GetSymbol("_ZN10Reanimation13OverrideScaleEff");
    LawnApp_PlaySampleAddr = libGameMain.GetSymbol("_ZN7LawnApp10PlaySampleEib");

    // TodFoley hook addresses
    TodFoley_PlayFoleyAddr = libGameMain.GetSymbol("_ZN8TodFoley9PlayFoleyE9FoleyType");
    TodFoley_PlayFoleyPitchAddr = libGameMain.GetSymbol("_ZN8TodFoley14PlayFoleyPitchE9FoleyTypef");

    // gSexyAppBase: Sexy::SexyAppBase* stored at _ZN4Sexy12gSexyAppBaseE
    if (void *p = libGameMain.GetSymbol("_ZN4Sexy12gSexyAppBaseE"))
        Sexy_gSexyAppBase_ptr = reinterpret_cast<Sexy::SexyAppBase **>(p);
    // gFoleyParamArray: FoleyParams* at symbol "gFoleyParamArray"
    if (void *p = libGameMain.GetSymbol("gFoleyParamArray"))
        gFoleyParamArray_ptr = reinterpret_cast<FoleyParams **>(p);
    // gFoleyParamArraySize: int at symbol "gFoleyParamArraySize"
    if (void *p = libGameMain.GetSymbol("gFoleyParamArraySize"))
        gFoleyParamArraySize_ptr = reinterpret_cast<int *>(p);
    if (void *p = libGameMain.GetSymbol("gInAssert"))
        gInAssert_ptr = reinterpret_cast<char *>(p);
    if (void *p = libGameMain.GetSymbol("_assert2"))
        _assert2_ptr = reinterpret_cast<void (*)(const char *, int, const char *, const char *)>(p);

    void *soundFlowerAddr = libGameMain.GetSymbol("_ZN4Sexy23SOUND_LOADINGBAR_FLOWERE");
    void *soundZombieAddr = libGameMain.GetSymbol("_ZN4Sexy23SOUND_LOADINGBAR_ZOMBIEE");
    if (soundFlowerAddr) Sexy_SOUND_LOADINGBAR_FLOWER = soundFlowerAddr;
    if (soundZombieAddr) Sexy_SOUND_LOADINGBAR_ZOMBIE = soundZombieAddr;

    Sexy_RandIntAddr = libGameMain.GetSymbol("_ZN4Sexy4RandEi");
    Sexy_RandFloatAddr = libGameMain.GetSymbol("_ZN4Sexy4RandEf");

    TodFoley_IsFoleyPlayingAddr = libGameMain.GetSymbol("_ZN8TodFoley14IsFoleyPlayingE9FoleyType");
    TodFoley_StopFoleyAddr = libGameMain.GetSymbol("_ZN8TodFoley9StopFoleyE9FoleyType");
    TodFoley_CancelPausedFoleyAddr = libGameMain.GetSymbol("_ZN8TodFoley17CancelPausedFoleyEv");
    TodFoley_GamePauseAddr = libGameMain.GetSymbol("_ZN8TodFoley9GamePauseEb");
    TodFoley_RehookupSoundWithMusicVolumeAddr = libGameMain.GetSymbol("_ZN8TodFoley28RehookupSoundWithMusicVolumeEv");
    TodFoleyInitializeAddr = libGameMain.GetSymbol("_Z18TodFoleyInitializeP11FoleyParamsi");

    LOG_INFO("TitleScreen_DrawAddr: {}", static_cast<void *>(TitleScreen_DrawAddr));
    LOG_INFO("TodStringTranslateAddr: {}", static_cast<void *>(TodStringTranslateAddr));
    LOG_INFO("TodDrawStringAddr: {}", static_cast<void *>(TodDrawStringAddr));
    LOG_INFO("Sexy_Graphics_DrawStringAddr: {}", static_cast<void *>(Sexy_Graphics_DrawStringAddr));
    LOG_INFO("Sexy_Graphics_SetFontAddr: {}", static_cast<void *>(Sexy_Graphics_SetFontAddr));
    LOG_INFO("Sexy_Graphics_FillRectAddr: {}", static_cast<void *>(Sexy_Graphics_FillRectAddr));
    LOG_INFO("Sexy_Graphics_DrawImageAddr: {}", static_cast<void *>(Sexy_Graphics_DrawImageAddr));
    LOG_INFO("Sexy_Graphics_SetLinearBlendAddr: {}", static_cast<void *>(Sexy_Graphics_SetLinearBlendAddr));
    LOG_INFO("Sexy_Graphics_SetColorizeImagesAddr: {}", static_cast<void *>(Sexy_Graphics_SetColorizeImagesAddr));
    LOG_INFO("Sexy_Widget_DeferOverlayAddr: {}", static_cast<void *>(Sexy_Widget_DeferOverlayAddr));
    LOG_INFO("Sexy_Graphics_DrawImage3Addr: {}", static_cast<void *>(Sexy_Graphics_DrawImage3Addr));
    LOG_INFO("Sexy_Graphics_DrawImageMirror2Addr: {}", static_cast<void *>(Sexy_Graphics_DrawImageMirror2Addr));
    LOG_INFO("TodAnimateCurveAddr: {}", static_cast<void *>(TodAnimateCurveAddr));
    LOG_INFO("TodAnimateCurveFloatTimeAddr: {}", static_cast<void *>(TodAnimateCurveFloatTimeAddr));
    LOG_INFO("TodScaleRotateTransformMatrixAddr: {}", static_cast<void *>(TodScaleRotateTransformMatrixAddr));
    LOG_INFO("TodBltMatrixAddr: {}", static_cast<void *>(TodBltMatrixAddr));
    LOG_INFO("Sexy_Graphics_GraphicsAddr: {}", static_cast<void *>(Sexy_Graphics_GraphicsAddr));
    LOG_INFO("Sexy_Graphics_DeleteAddr: {}", static_cast<void *>(Sexy_Graphics_DeleteAddr));
    LOG_INFO("Sexy_Graphics_ClipRectAddr: {}", static_cast<void *>(Sexy_Graphics_ClipRectAddr));
    LOG_INFO("Sexy_GetTickCountAddr: {}", static_cast<void *>(Sexy_GetTickCountAddr));
    return true;
}

