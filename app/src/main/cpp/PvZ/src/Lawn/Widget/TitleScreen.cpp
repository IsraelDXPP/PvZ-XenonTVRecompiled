#include "PvZ/Lawn/Widget/TitleScreen.h"
#include "PvZ/Symbols.h"
#include "PvZ/TodLib/Common/TodCommon.h"
#include "PvZ/SexyAppFramework/Misc/Common.h"
#include "PvZ/SexyAppFramework/Graphics/Color.h"
#include <cmath>
#include <sys/time.h>
#include <ctype.h>
#include <android/log.h>


static int GetTickCountMs() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

static void *ReadPtr(void *obj, int offset) {
    return *reinterpret_cast<void **>(static_cast<char *>(obj) + offset);
}

static int ReadInt(void *obj, int offset) {
    return *reinterpret_cast<int *>(static_cast<char *>(obj) + offset);
}

static unsigned char ReadByte(void *obj, int offset) {
    return *reinterpret_cast<unsigned char *>(static_cast<char *>(obj) + offset);
}

static float ReadFloat(void *obj, int offset) {
    return *reinterpret_cast<float *>(static_cast<char *>(obj) + offset);
}

static void WriteByte(void *obj, int offset, unsigned char val) {
    *reinterpret_cast<unsigned char *>(static_cast<char *>(obj) + offset) = val;
}

static void WriteInt(void *obj, int offset, int val) {
    *reinterpret_cast<int *>(static_cast<char *>(obj) + offset) = val;
}

static void WriteFloat(void *obj, int offset, float val) {
    *reinterpret_cast<float *>(static_cast<char *>(obj) + offset) = val;
}

// Global Callback for Video playback
static void *TitleScreen_VideoCallback(void *result) {
    unsigned int val = *reinterpret_cast<unsigned int *>(result);
    if (val - 13 <= 1) {
        auto agViewShow = reinterpret_cast<void (*)(int)>(AGViewShowAddr);
        auto agVideoShow = reinterpret_cast<void (*)(int)>(AGVideoShowAddr);
        if (agViewShow) agViewShow(1);
        if (agVideoShow) agVideoShow(0);
    }
    return result;
}

static void TitleScreen_VideoClose() {
    auto removeListener = reinterpret_cast<void (*)(void*)>(AGViewRemoveEventListenerAddr);
    auto videoClose = reinterpret_cast<void (*)()>(AGVideoCloseAddr);
    auto agViewShow = reinterpret_cast<void (*)(int)>(AGViewShowAddr);
    auto agVideoShow = reinterpret_cast<void (*)(int)>(AGVideoShowAddr);

    if (removeListener) removeListener(reinterpret_cast<void*>(TitleScreen_VideoCallback));
    if (videoClose) videoClose();
    if (agViewShow) agViewShow(1);
    if (agVideoShow) agVideoShow(0);
}


void TitleScreen_Draw(TitleScreen *titleScreen, Sexy::Graphics *graphics) {
    graphics->SetLinearBlend(true);

    auto widgetDraw = reinterpret_cast<void (*)(void *, Sexy::Graphics *)>(Sexy_Widget_DrawAddr);
    if (widgetDraw) {
        widgetDraw(titleScreen, graphics);
    }

    int state = ReadInt(titleScreen, 0x130);
    int counter = ReadInt(titleScreen, 0x134);

    // ===== State 0: WaitingForFirstDraw =====
    if (state == TitleScreen::WaitingForFirstDraw) {
        graphics->SetColor(Sexy::Color(0, 0, 0, 255));
        graphics->FillRect(Sexy::Rect(0, 0, ReadInt(titleScreen, 60), ReadInt(titleScreen, 64)));

        if (!ReadByte(titleScreen, 0x128)) {
            if (Sexy_GetTickCountAddr) {
                reinterpret_cast<int (*)()>(Sexy_GetTickCountAddr)();
            } else {
                GetTickCountMs();
            }
            WriteByte(titleScreen, 0x128, 1);
        }
        return;
    }

    // ===== State 1: Logo / Intro Video =====
    if (state == 1) {
        bool videoCompleted = ReadByte(titleScreen, 318);
        if (videoCompleted) {
            graphics->SetColor(Sexy::Color(0, 0, 0, 0));
            graphics->ClearRect(-240, -60, 1280, 720);
        } else {
            Sexy::Image *popcapLogo = reinterpret_cast<Sexy::Image *>(ReadPtr(titleScreen, 0x104));
            graphics->SetColor(Sexy::Color(0, 0, 0, 255));
            graphics->FillRect(Sexy::Rect(-240, -60, 1280, 720));

            int alphaVal = TodAnimateCurve(30, 0, counter, 255, 0, static_cast<TodCurves>(1));

            auto setColorizeImages = reinterpret_cast<void (*)(Sexy::Graphics *, bool)>(Sexy_Graphics_SetColorizeImagesAddr);
            if (setColorizeImages) {
                setColorizeImages(graphics, true);
            }
            graphics->SetColor(Sexy::Color(255, 255, 255, alphaVal));

            if (popcapLogo) {
                int logoW = ReadInt(popcapLogo, 28);
                int logoH = ReadInt(popcapLogo, 32);
                int screenH = ReadInt(titleScreen, 64); // Widget height

                float scaleH = (static_cast<float>(screenH) * 0.3333f);
                float drawW = scaleH * (static_cast<float>(logoW) / static_cast<float>(logoH));
                int drawX = static_cast<int>((static_cast<float>(ReadInt(titleScreen, 60)) / 2.0f) + (drawW * -0.5f));
                int drawY = static_cast<int>((static_cast<float>(ReadInt(titleScreen, 64)) / 2.0f) + (scaleH * -0.5f));

                auto drawImage3 = reinterpret_cast<void (*)(Sexy::Graphics *, Sexy::Image *, int, int, int, int)>(Sexy_Graphics_DrawImage3Addr);
                if (drawImage3) {
                    drawImage3(graphics, popcapLogo, drawX, drawY, static_cast<int>(drawW), static_cast<int>(scaleH));
                }
            }

            int titleStateDur = ReadInt(titleScreen, 0x138);
            int fadeOutAlpha = TodAnimateCurve(0, 30, titleStateDur - counter, 255, 0, static_cast<TodCurves>(1));
            graphics->SetColor(Sexy::Color(255, 255, 255, fadeOutAlpha));
            graphics->FillRect(Sexy::Rect(-240, -60, 1280, 720));

            if (setColorizeImages) {
                setColorizeImages(graphics, false);
            }
        }
        return;
    }

    // ===== State 2: ESRB Rating =====
    if (state == 2) {
        Sexy::Image *esrbLogo = GetImageEsrbRating();
        graphics->SetColor(Sexy::Color(0, 0, 0, 255));
        graphics->FillRect(Sexy::Rect(-240, -60, 1280, 720));

        int alphaVal = TodAnimateCurve(30, 0, counter, 255, 0, static_cast<TodCurves>(1));

        auto setColorizeImages = reinterpret_cast<void (*)(Sexy::Graphics *, bool)>(Sexy_Graphics_SetColorizeImagesAddr);
        if (setColorizeImages) {
            setColorizeImages(graphics, true);
        }
        graphics->SetColor(Sexy::Color(255, 255, 255, alphaVal));

        if (esrbLogo) {
            int logoW = ReadInt(esrbLogo, 28);
            int logoH = ReadInt(esrbLogo, 32);
            int screenH = ReadInt(titleScreen, 64); // Widget height

            float scaleH = (static_cast<float>(screenH) * 0.65f);
            float drawW = scaleH * (static_cast<float>(logoW) / static_cast<float>(logoH));
            int drawX = static_cast<int>((static_cast<float>(ReadInt(titleScreen, 60)) / 2.0f) + (drawW * -0.5f));
            int drawY = static_cast<int>((static_cast<float>(ReadInt(titleScreen, 64)) / 2.0f) + (scaleH * -0.5f));

            auto drawImage3 = reinterpret_cast<void (*)(Sexy::Graphics *, Sexy::Image *, int, int, int, int)>(Sexy_Graphics_DrawImage3Addr);
            if (drawImage3) {
                drawImage3(graphics, esrbLogo, drawX, drawY, static_cast<int>(drawW), static_cast<int>(scaleH));
            }
        }

        int titleStateDur = ReadInt(titleScreen, 0x138);
        int fadeOutAlpha = TodAnimateCurve(0, 30, titleStateDur - counter, 255, 0, static_cast<TodCurves>(1));
        graphics->SetColor(Sexy::Color(255, 255, 255, fadeOutAlpha));
        graphics->FillRect(Sexy::Rect(-240, -60, 1280, 720));

        if (setColorizeImages) {
            setColorizeImages(graphics, false);
        }
        return;
    }

    // ===== State 3: Guide Screen =====
    if (state == 3) {
        Sexy::Image *guideImg = reinterpret_cast<Sexy::Image *>(ReadPtr(titleScreen, 0x108));
        graphics->SetColor(Sexy::Color(0, 0, 0, 255));
        graphics->FillRect(Sexy::Rect(-240, -60, 1280, 720));

        int alphaVal = TodAnimateCurve(30, 0, counter, 255, 0, static_cast<TodCurves>(1));

        auto setColorizeImages = reinterpret_cast<void (*)(Sexy::Graphics *, bool)>(Sexy_Graphics_SetColorizeImagesAddr);
        if (setColorizeImages) {
            setColorizeImages(graphics, true);
        }
        graphics->SetColor(Sexy::Color(255, 255, 255, alphaVal));

        if (guideImg) {
            int logoW = ReadInt(guideImg, 28);
            int logoH = ReadInt(guideImg, 32);
            int screenH = ReadInt(titleScreen, 64); // Widget height

            float scaleH = static_cast<float>(logoH);
            if (static_cast<float>(logoH) < (static_cast<float>(screenH) * 0.45f)) {
                scaleH = static_cast<float>(screenH) * 0.5f;
            }
            float drawW = scaleH * (static_cast<float>(logoW) / static_cast<float>(logoH));
            int drawX = static_cast<int>((static_cast<float>(ReadInt(titleScreen, 60)) / 2.0f) + (drawW * -0.5f));
            int drawY = static_cast<int>((static_cast<float>(ReadInt(titleScreen, 64)) / 2.0f) + (scaleH * -0.5f));

            auto drawImage3 = reinterpret_cast<void (*)(Sexy::Graphics *, Sexy::Image *, int, int, int, int)>(Sexy_Graphics_DrawImage3Addr);
            if (drawImage3) {
                drawImage3(graphics, guideImg, drawX, drawY, static_cast<int>(drawW), static_cast<int>(scaleH));
            }
        }

        int titleStateDur = ReadInt(titleScreen, 0x138);
        int fadeOutAlpha = TodAnimateCurve(0, 30, titleStateDur - counter, 255, 0, static_cast<TodCurves>(1));
        graphics->SetColor(Sexy::Color(255, 255, 255, fadeOutAlpha));
        graphics->FillRect(Sexy::Rect(-240, -60, 1280, 720));

        if (setColorizeImages) {
            setColorizeImages(graphics, false);
        }
        return;
    }

    // ===== State 4+: Loading screen =====
    if (!ReadByte(titleScreen, 0x13c)) {
        graphics->SetColor(Sexy::Color(0, 0, 0, 255));
        graphics->FillRect(Sexy::Rect(0, 0, ReadInt(titleScreen, 60), ReadInt(titleScreen, 64)));
        return;
    }

    if (ReadByte(titleScreen, 0x129)) {
        return;
    }

    int scrH = ReadInt(titleScreen, 64);

    Sexy::Image *bg = GetTitleScreenImage();
    if (bg) {
        graphics->DrawImage(bg, -240, -60);
    }

    int titleAge = ReadInt(titleScreen, 0x124);
    float sinVal = (std::sin(titleAge * 0.035f) + 1.0f) * 0.5f;
    if (sinVal > 0.7f) {
        Sexy::Image *ballImg = GetImageTitleScreenBall();
        Sexy::Image *glowImg = GetImageTitleScreenGlow();

        if (ballImg) {
            graphics->DrawImage(ballImg, -240 + 682, -60 + 325);
        }

        auto setColorizeImages = reinterpret_cast<void (*)(Sexy::Graphics *, bool)>(Sexy_Graphics_SetColorizeImagesAddr);
        if (setColorizeImages && glowImg) {
            setColorizeImages(graphics, true);
            int alpha = static_cast<int>((sinVal - 0.5f) * 255.0f);
            graphics->SetColor(Sexy::Color(255, 255, 255, alpha));
            graphics->DrawImage(glowImg, -240 + 575, -60 + 213);
            setColorizeImages(graphics, false);
        } else if (glowImg) {
            graphics->DrawImage(glowImg, -240 + 575, -60 + 213);
        }
    }

    Sexy::Image *pvzLogo = GetImagePvzLogo();
    if (pvzLogo) {
        int logoY;
        if (counter <= 60) {
            logoY = TodAnimateCurve(60, 50, counter, 10, 15, static_cast<TodCurves>(9));
        } else {
            logoY = TodAnimateCurve(100, 60, counter, -200, 10, static_cast<TodCurves>(2));
        }
        graphics->DrawImage(pvzLogo, 50, logoY);
    }

    void *startBtn = ReadPtr(titleScreen, 0x10c);
    float curW = ReadFloat(titleScreen, 0x110);
    float totW = ReadFloat(titleScreen, 0x114);

    int barY;
    int dirtY;
    if (startBtn) {
        int btnVal = ReadInt(startBtn, 56);
        dirtY = btnVal + 1;
        barY = btnVal - 17;
    } else {
        dirtY = scrH - 60;
        barY = dirtY - 18;
    }

    Sexy::Image *ldDirt = GetImageLoadBarDirt();
    Sexy::Image *ldGrass = GetImageLoadBarGrass();
    Sexy::Image *sodroll = GetImageReanimSodrollCap();

    if (ldDirt) {
        graphics->DrawImage(ldDirt, 168, dirtY);
    }

    if (ldGrass && totW > 0.0f) {
        int grassH = ReadInt(ldGrass, 32);
        if (curW < totW) {
            char copyBuf[100];
            auto gfxCtor = reinterpret_cast<void (*)(void *, const Sexy::Graphics *)>(Sexy_Graphics_GraphicsAddr);
            auto gfxDtor = reinterpret_cast<void (*)(void *)>(Sexy_Graphics_DeleteAddr);
            auto gfxClipRect = reinterpret_cast<void (*)(void *, int, int, int, int)>(Sexy_Graphics_ClipRectAddr);
            auto gfxDrawImg = reinterpret_cast<void (*)(void *, Sexy::Image *, int, int)>(Sexy_Graphics_DrawImageAddr);

            gfxCtor(copyBuf, graphics);
            gfxClipRect(copyBuf, 164, barY, static_cast<int>(curW), grassH);
            gfxDrawImg(copyBuf, ldGrass, 164, barY);

            float v50 = curW * 0.94f;
            float v51 = TodAnimateCurveFloatTime(0.0f, totW, curW, 1.2f, 0.8f, static_cast<TodCurves>(1));
            float transX = ReadFloat(graphics, 8);
            float transY = ReadFloat(graphics, 12);

            Sexy::SexyTransform2D xform;
            TodScaleRotateTransformMatrix(
                xform,
                v50 + 175.0f + transX,
                static_cast<float>(barY) + 6.0f + v51 * -35.0f + 35.0f + transY,
                (v50 / -180.0f) * 6.2832f,
                v51,
                v51);

            if (sodroll && TodBltMatrixAddr) {
                int sx = ReadInt(sodroll, 28);
                int sy = ReadInt(sodroll, 32);
                Sexy::Rect srcRect(0, 0, sx, sy);
                int drawMode = ReadInt(graphics, 68);
                auto bltMatrix = reinterpret_cast<void (*)(
                    Sexy::Graphics *, Sexy::Image *, const Sexy::SexyMatrix3 &,
                    const Sexy::Rect &, const Sexy::Color &, int, const Sexy::Rect &)>(TodBltMatrixAddr);
                bltMatrix(
                    graphics, sodroll, xform,
                    *reinterpret_cast<const Sexy::Rect *>(reinterpret_cast<const char *>(graphics) + 32),
                    Sexy::Color(255, 255, 255, 255),
                    drawMode,
                    srcRect);
            }

            gfxDtor(copyBuf);
        } else {
            graphics->DrawImage(ldGrass, 164, barY);
        }
    }

    void *app = ReadPtr(titleScreen, 0x14c);
    if (app && Reanimation_DrawAddr) {
        // DataArray iterator pattern: *(app + 1980) = ReanimationHolder, *(holder + 8) = DataArray block
        void *holder = ReadPtr(app, 1980);
        if (holder) {
            void *block = ReadPtr(holder, 8);
            if (block) {
                char *base = *reinterpret_cast<char **>(block);     // block[0] = mBlock pointer
                int maxSize = *reinterpret_cast<int *>(static_cast<char *>(block) + 4); // block[4] = mMaxSize
                if (base && maxSize > 0) {
                    char *end = base + static_cast<unsigned int>(maxSize) * 192;
                    auto reanimDraw = reinterpret_cast<void (*)(void *, Sexy::Graphics *)>(Reanimation_DrawAddr);
                    for (char *cur = base; reinterpret_cast<unsigned int>(cur) < reinterpret_cast<unsigned int>(end); cur += 192) {
                        // check mVisible flag at offset 0xBE (190)
                        if (*reinterpret_cast<unsigned short *>(cur + 0xbe)) {
                            reanimDraw(cur, graphics);
                        }
                    }
                }
            }
        }
    }

    auto deferOverlay = reinterpret_cast<void (*)(void *, int)>(Sexy_Widget_DeferOverlayAddr);
    if (deferOverlay) {
        deferOverlay(titleScreen, 0);
    }
}

void TitleScreen_Update(TitleScreen *titleScreen) {
    auto widgetUpdate = reinterpret_cast<void (*)(void *)>(Sexy_Widget_UpdateAddr);
    if (widgetUpdate) {
        widgetUpdate(titleScreen);
    }

    void *app = ReadPtr(titleScreen, 0x14C);
    if (!app) return;

    if (ReadByte(app, 689)) {
        return;
    }

    auto agVideoIsPlaying = reinterpret_cast<int (*)()>(AGVideoIsPlayingAddr);
    bool isPlaying = agVideoIsPlaying ? (agVideoIsPlaying() != 0) : false;

    if (!isPlaying) {
        auto vtable = *reinterpret_cast<void ***>(titleScreen);
        auto func = reinterpret_cast<void (*)(TitleScreen *)>(vtable[23]);
        if (func) func(titleScreen);
    }

    if (!ReadByte(titleScreen, 296)) {
        return;
    }

    int state = ReadInt(titleScreen, 0x130);
    if (state == 0) {
        auto startLoadingThread = reinterpret_cast<void (*)(void *)>(Sexy_SexyAppBase_StartLoadingThreadAddr);
        if (startLoadingThread) {
            startLoadingThread(app);
        }

        if (ReadByte(titleScreen, 328)) {
            TitleScreen_SwitchState(titleScreen, TitleScreen::PopcapLogo_OR_IntroVideo, 500);
            
            auto getEnvOption = reinterpret_cast<bool (*)(const char *, const char *, bool)>(Sexy_GetEnvOptionAddr);
            bool noIntro = getEnvOption ? getEnvOption("LAWN_NO_INTRO_VIDEO", nullptr, false) : false;
            
            bool videoFailed = true;
            if (!noIntro) {
                auto videoShow = reinterpret_cast<int (*)(int)>(AGVideoShowAddr);
                auto videoOpen = reinterpret_cast<int (*)(const char *)>(AGVideoOpenAddr);
                auto videoPlay = reinterpret_cast<int (*)()>(AGVideoPlayAddr);
                
                if (videoShow && videoOpen && videoPlay) {
                    videoShow(1);
                    if (videoOpen("movies/intro.mp4") == 0) {
                        if (videoPlay() == 0) {
                            videoFailed = false;
                        }
                    }
                }
            }

            if (videoFailed) {
                WriteByte(titleScreen, 318, 0);
            } else {
                auto addListener = reinterpret_cast<int (*)(void*)>(AGViewAddEventListenerAddr);
                if (addListener) {
                    int listenerId = addListener(reinterpret_cast<void*>(TitleScreen_VideoCallback));
                    
                    auto getModel = reinterpret_cast<char* (*)(int)>(AGGetModelAddr);
                    std::string modelStr = "";
                    if (getModel) {
                        char *m = getModel(listenerId);
                        if (m) modelStr = m;
                    }
                    
                    std::string upperModel = "";
                    for (char c : modelStr) {
                        upperModel += toupper(c);
                    }
                    
                    int sdkVer = 0;
                    auto getSdkVersion = reinterpret_cast<int (*)()>(AGGetSdkVersionAddr);
                    if (getSdkVersion) sdkVer = getSdkVersion();
                    
                    if (upperModel.find("IDEA") != std::string::npos &&
                        upperModel.find("TV") != std::string::npos &&
                        sdkVer <= 13) {
                        auto agViewShow = reinterpret_cast<void (*)(int)>(AGViewShowAddr);
                        if (agViewShow) agViewShow(0);
                    }
                }
                WriteByte(titleScreen, 318, 1);
            }
            state = ReadInt(titleScreen, 0x130);
        } else {
            TitleScreen_SwitchState(titleScreen, TitleScreen::Loading, 100);
            state = ReadInt(titleScreen, 0x130);
        }
    }

    bool isCompleted = ReadByte(titleScreen, 289);
    int titleAge = ReadInt(titleScreen, 0x124);
    WriteInt(titleScreen, 0x124, titleAge + 1);

    if (isCompleted) {
        void *startBtn = ReadPtr(titleScreen, 0x10c);
        if (startBtn) {
            WriteInt(startBtn, 748, 2);
        }
    }

    int counter = ReadInt(titleScreen, 0x134);
    if (counter > 0) {
        WriteInt(titleScreen, 0x134, counter - 1);
    }

    if (state == 2) {
        if (ReadInt(titleScreen, 0x134) == 0) {
            TitleScreen_SwitchState(titleScreen, TitleScreen::Loading, 100);
        }
        return;
    }

    if (state == 1) {
        bool videoCompleted = ReadByte(titleScreen, 318);
        if (!videoCompleted || isPlaying) {
            if (ReadInt(titleScreen, 0x134) == 0) {
                if (videoCompleted && isPlaying) {
                    int duration = ReadInt(titleScreen, 0x138);
                    WriteInt(titleScreen, 0x138, duration + 100);
                    WriteInt(titleScreen, 0x134, counter + 100);
                    return;
                }
            }
        } else {
            TitleScreen_VideoClose();
            int age = ReadInt(titleScreen, 0x124);
            WriteByte(titleScreen, 318, 0);
            if (age <= 99) {
                WriteInt(titleScreen, 0x124, age + 500);
            }
        }

        void *guide = ReadPtr(titleScreen, 0x108);
        if (guide) {
            TitleScreen_SwitchState(titleScreen, TitleScreen::GuideLogo, 400);
            return;
        }

        TitleScreen_SwitchState(titleScreen, TitleScreen::Loading, 100);
        return;
    }

    if (state == 3 && ReadInt(titleScreen, 0x134) == 0) {
        TitleScreen_SwitchState(titleScreen, TitleScreen::Loading, 100);
    }

    if (!ReadByte(titleScreen, 316)) {
        return;
    }

    if (!ReadByte(titleScreen, 317)) {
        void *music = ReadPtr(app, 0x7D8);
        if (music) {
            auto playMusic = reinterpret_cast<void (*)(void *, int)>(reinterpret_cast<void**>(*reinterpret_cast<void***>(music))[14]);
            if (playMusic) {
                playMusic(music, 7);
            }
        }
        WriteByte(titleScreen, 317, 1);
    }

    float loadingPercent = 3000.0f;
    auto getLoadingPercentFunc = reinterpret_cast<float (*)(void *)>(reinterpret_cast<void**>(*reinterpret_cast<void***>(app))[84]);
    if (getLoadingPercentFunc) {
        loadingPercent = getLoadingPercentFunc(app);
    }

    float totalW = ReadFloat(titleScreen, 0x114);
    float curW = ReadFloat(titleScreen, 0x110);
    float barVel = ReadFloat(titleScreen, 0x118);

    if (ReadByte(titleScreen, 297) && ReadInt(titleScreen, 0x130) == 4) {
        WriteByte(titleScreen, 297, 0);
        
        void *startBtn = ReadPtr(titleScreen, 0x10c);
        if (startBtn) {
            std::string loadingStr = "";
            auto translate = reinterpret_cast<void (*)(std::string *, const char *)>(TodStringTranslateAddr);
            if (translate) {
                translate(&loadingStr, "[LOADING]");
            }
            
            auto setPhrase = reinterpret_cast<void (*)(void *, const std::string &)>(TypingCheck_SetPhraseAddr);
            if (setPhrase) {
                setPhrase(static_cast<char*>(startBtn) + 740, loadingStr);
            }
            
            auto setFont = reinterpret_cast<void (*)(void *, Sexy::Font *)>(reinterpret_cast<void**>(*reinterpret_cast<void***>(startBtn))[113]);
            if (setFont) {
                setFont(startBtn, GetFontDwarvenTodcraft24());
            }
            
            auto resize = reinterpret_cast<void (*)(void *, int, int, int, int)>(reinterpret_cast<void**>(*reinterpret_cast<void***>(startBtn))[48]);
            if (resize) {
                resize(startBtn, 182, 800, static_cast<int>(totalW), 64);
            }
            WriteByte(startBtn, 116, 1);
        }

        int curAge = ReadInt(titleScreen, 0x124);
        float v7 = loadingPercent > 0.000001f ? (static_cast<float>(curAge) / loadingPercent) : 3000.0f;
        float v8 = (1.0f - loadingPercent) * v7;
        if (v8 > 100.0f) {
            if (v8 >= 3000.0f) v8 = 3000.0f;
        } else {
            v8 = 100.0f;
        }
        float v9 = 0.9f;
        WriteFloat(titleScreen, 0x118, totalW / v8);
        if (loadingPercent <= 0.9f) {
            v9 = loadingPercent;
        }
        WriteFloat(titleScreen, 0x11c, v9);
    }

    int stateCounter = ReadInt(titleScreen, 0x134);
    int v13;
    if (stateCounter > 10) {
        v13 = TodAnimateCurve(120, 10, stateCounter, 800, 500, static_cast<TodCurves>(2));
    } else {
        v13 = TodAnimateCurve(10, 0, stateCounter, 500, 529, static_cast<TodCurves>(9));
    }

    void *startBtn = ReadPtr(titleScreen, 0x10c);
    if (startBtn) {
        auto resize = reinterpret_cast<void (*)(void *, int, int, int, int)>(reinterpret_cast<void**>(*reinterpret_cast<void***>(startBtn))[48]);
        if (resize) {
            int btnX = ReadInt(startBtn, 52);
            int btnH = ReadInt(startBtn, 64);
            resize(startBtn, btnX, v13, static_cast<int>(totalW), btnH);
        }
    }

    if (ReadInt(titleScreen, 0x134) > 0) {
        return;
    }

    void *effectSys = ReadPtr(app, 1980);
    if (effectSys && EffectSystem_UpdateAddr) {
        auto effectUpdate = reinterpret_cast<void (*)(void *)>(EffectSystem_UpdateAddr);
        effectUpdate(effectSys);
    }

    float newBarW = curW + ReadFloat(titleScreen, 0x118);
    WriteFloat(titleScreen, 0x110, newBarW);

    bool loadingThreadComplete = ReadByte(titleScreen, 289);
    if (loadingThreadComplete) {
        if (newBarW > totalW) {
            void *startBtn = ReadPtr(titleScreen, 0x10c);
            if (startBtn) {
                std::string clickStartStr = "";
                auto translate = reinterpret_cast<void (*)(std::string *, const char *)>(TodStringTranslateAddr);
                if (translate) {
                    translate(&clickStartStr, "[CLICK_TO_START]");
                }
                
                auto setPhrase = reinterpret_cast<void (*)(void *, const std::string &)>(TypingCheck_SetPhraseAddr);
                if (setPhrase) {
                    setPhrase(static_cast<char*>(startBtn) + 740, clickStartStr);
                }
            }
            WriteFloat(titleScreen, 0x110, totalW);
        }
    } else {
        if (newBarW > (totalW * 0.99f)) {
            WriteFloat(titleScreen, 0x110, totalW * 0.99f);
        }
    }

    float v18 = (loadingPercent - ReadFloat(titleScreen, 0x11c)) / (1.0f - ReadFloat(titleScreen, 0x11c));
    isCompleted = ReadByte(titleScreen, 289);
    
    if (v18 <= (ReadFloat(titleScreen, 0x12c) + 0.01f) && !isCompleted) {
        if (ReadByte(app, 1262)) {
            void *startBtn = ReadPtr(titleScreen, 0x10c);
            if (startBtn) {
                WriteByte(titleScreen, 289, 1);
                auto setDisabled = reinterpret_cast<void (*)(void *, bool)>(reinterpret_cast<void**>(*reinterpret_cast<void***>(startBtn))[46]);
                if (setDisabled) {
                    setDisabled(startBtn, false);
                }
                auto setVisible = reinterpret_cast<void (*)(void *, bool)>(reinterpret_cast<void**>(*reinterpret_cast<void***>(startBtn))[40]);
                if (setVisible) {
                    setVisible(startBtn, true);
                }
            }
        }
    } else {
        float newProgress = TodAnimateCurveFloatTime(0.0f, 1.0f, v18, 0.0f, totalW, static_cast<TodCurves>(2)) - ReadFloat(titleScreen, 0x110);
        float accel = TodAnimateCurveFloatTime(0.0f, 1.0f, v18, 0.0001f, 0.00001f, static_cast<TodCurves>(1));
        float factor = 0.0001f;
        if (!isCompleted) {
            factor = accel;
        }
        float absDiff = newProgress < 0.0f ? -newProgress : newProgress;
        float newVel = ReadFloat(titleScreen, 0x118) + (absDiff * newProgress * factor);
        WriteFloat(titleScreen, 0x118, newVel);
        
        float limit = TodAnimateCurveFloatTime(0.0f, 1.0f, v18, 0.2f, 0.01f, static_cast<TodCurves>(1));
        float maxVel = ReadByte(app, 1937) ? 100.0f : 2.0f;
        float minVel = ReadByte(app, 1937) ? 0.0f : limit;
        
        if (newVel >= minVel) {
            if (newVel > maxVel) {
                WriteFloat(titleScreen, 0x118, maxVel);
            }
        } else {
            WriteFloat(titleScreen, 0x118, minVel);
        }
    }
    
    WriteFloat(titleScreen, 0x12c, v18);

    // Spawn reanimations at loading bar thresholds (LABEL_47 logic)
    // 5 thresholds: 11%, 32%, 54%, 72%, 95% of totalW
    // Even indices (0,2,4) = sprout/flower; odd (1,3) = zombiehead; index 4 = zombiehead (mirrored)
    // Types: 0=sprout, 1=zombieHead, 2=sprout2, 3=zombieHead mirrored, 4=zombiehead2
    if (LawnApp_AddReanimationAddr) {
        float prevBarW = newBarW - ReadFloat(titleScreen, 0x118);
        float thresholds[5];
        thresholds[0] = totalW * 0.11f;
        thresholds[1] = totalW * 0.32f;
        thresholds[2] = totalW * 0.54f;
        thresholds[3] = totalW * 0.72f;
        thresholds[4] = totalW * 0.95f;

        auto addReanim = reinterpret_cast<void *(*)(void *, float, float, int, int)>(LawnApp_AddReanimationAddr);
        auto overrideScale = reinterpret_cast<void (*)(void *, float, float)>(Reanimation_OverrideScaleAddr);
        auto playSample = reinterpret_cast<void (*)(void *, int, bool)>(LawnApp_PlaySampleAddr);

        for (int idx = 0; idx < 5; idx++) {
            float thresh = thresholds[idx];
            // trigger when bar crosses threshold this frame
            if (prevBarW < thresh && newBarW >= thresh) {
                float spawnX = thresh + 164.0f - 24.0f;
                // type 1 = Zombiehead (only for idx == 4), 0 = Sprout (for idx == 0, 1, 2, 3)
                int reanimType = (idx == 4) ? 1 : 0;

                void *reanim = addReanim(app, spawnX, 481.0f, 0, reanimType);
                if (reanim) {
                    // set mAnimRate = 18.0f (0x41900000)
                    *reinterpret_cast<float *>(static_cast<char *>(reanim) + 24) = 18.0f;
                    // set mReanimType = 3
                    *reinterpret_cast<int *>(static_cast<char *>(reanim) + 32) = 3;

                    if (idx == 1 || idx == 3) {
                        // mirror sprout: OverrideScale(-1, 1)
                        if (overrideScale) overrideScale(reanim, -1.0f, 1.0f);
                    } else if (idx == 2) {
                        // sprout idx 2: y position 476, OverrideScale(1.1, 1.3)
                        *reinterpret_cast<float *>(static_cast<char *>(reanim) + 236) = spawnX;
                        *reinterpret_cast<float *>(static_cast<char *>(reanim) + 248) = 476.0f;
                        if (overrideScale) overrideScale(reanim, 1.1f, 1.3f);
                    } else if (idx == 4) {
                        // zombiehead: shift position left
                        *reinterpret_cast<float *>(static_cast<char *>(reanim) + 236) = spawnX - 20.0f;
                        *reinterpret_cast<float *>(static_cast<char *>(reanim) + 248) = 481.0f;
                    }

                    // play sound
                    if (playSample) {
                        if (Sexy_SOUND_LOADINGBAR_FLOWER) {
                            int flowerSnd = *reinterpret_cast<int *>(Sexy_SOUND_LOADINGBAR_FLOWER);
                            playSample(app, flowerSnd, true);
                        }
                        if (idx == 4 && Sexy_SOUND_LOADINGBAR_ZOMBIE) {
                            int zombieSnd = *reinterpret_cast<int *>(Sexy_SOUND_LOADINGBAR_ZOMBIE);
                            playSample(app, zombieSnd, true);
                        }
                    }
                }
            }
        }
    }
}

void TitleScreen_SwitchState(TitleScreen *titleScreen, TitleScreen::TitleState state, int duration) {
    WriteInt(titleScreen, 0x130, state);
    WriteInt(titleScreen, 0x134, duration);
    WriteInt(titleScreen, 0x138, duration);
}