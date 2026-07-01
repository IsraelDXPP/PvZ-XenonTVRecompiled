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

#include <dlfcn.h>
#include <android/log.h>

#include "Homura/ExceptionUtils.h"
#include "Homura/MemoryUtils.h"
#include "PvZ/GlobalVariable.h"
#include "PvZ/HookInit.h"
#include "PvZ/Symbols.h"

#define LOG_TAG "HomuraMain"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

__attribute__((constructor)) static void LibMain() {
    LOGI("LibMain() called");

    homura::RegisterExceptionHandler();
    homura::RegisterAccessViolationHandler();

    // libGameLauncher might already be loaded by Java (System.loadLibrary)
    void* handle1 = dlopen("libGameLauncher.so", RTLD_LAZY | RTLD_NOLOAD);
    if (handle1) {
        LOGI("libGameLauncher.so already loaded (by Java)");
    } else {
        handle1 = dlopen("libGameLauncher.so", RTLD_NOW | RTLD_GLOBAL);
        if (handle1) {
            LOGI("libGameLauncher.so loaded via dlopen");
        } else {
            LOGE("dlopen(libGameLauncher) failed: %s", dlerror());
        }
    }

    void* handle2 = dlopen("libGameMain.so", RTLD_LAZY | RTLD_NOLOAD);
    if (handle2) {
        LOGI("libGameMain.so already loaded");
    } else {
        handle2 = dlopen("libGameMain.so", RTLD_NOW | RTLD_GLOBAL);
        if (handle2) {
            LOGI("libGameMain.so loaded via dlopen");
        } else {
            LOGE("dlopen(libGameMain) failed: %s", dlerror());
        }
    }

    gLibGameMainBaseAddr = homura::GetLibBaseAddr("libGameMain.so");
    if (gLibGameMainBaseAddr == 0) {
        LOGE("GetLibBaseAddr(libGameMain) returned 0");
    }

    if (InitSymbols()) {
        LOGI("InitSymbols() succeeded, calling CallHook()");
        CallHook();
    } else {
        LOGE("InitSymbols() failed");
    }
}


