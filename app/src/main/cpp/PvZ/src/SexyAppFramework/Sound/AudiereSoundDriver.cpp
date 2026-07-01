#include "PvZ/SexyAppFramework/Sound/AudiereSoundDriver.h"
#include "PvZ/SexyAppFramework/Sound/audiere/AudioTypes.h"
#include "PvZ/SexyAppFramework/SexyAppBase.h"

namespace Sexy {

static AudiereSoundManager *gSoundManager = nullptr;

AudiereSoundManager *AudiereSoundDriver::Create(SexyAppBase *app) {
    auto *manager = new AudiereSoundManager();
    auto *device = audiere::CreateAndroidAudioDevice(8192);
    manager->mAudioDevice = device;
    gSoundManager = manager;
    return manager;
}

void AudiereSoundDriver::Init() {
    if (!gSoundManager) {
        Create(nullptr);
    }
}

void AudiereSoundDriver::Shutdown() {
    if (gSoundManager) {
        delete gSoundManager;
        gSoundManager = nullptr;
    }
}

AudiereSoundManager *GetAudiereSoundManager() {
    return gSoundManager;
}

} // namespace Sexy
