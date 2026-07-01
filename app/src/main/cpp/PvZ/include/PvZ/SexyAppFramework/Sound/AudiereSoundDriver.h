#ifndef PVZ_SEXYAPPFRAMEWORK_SOUND_AUDIERE_SOUND_DRIVER_H
#define PVZ_SEXYAPPFRAMEWORK_SOUND_AUDIERE_SOUND_DRIVER_H

#include "AudiereSoundManager.h"

namespace Sexy {

class SexyAppBase;

class AudiereSoundDriver {
public:
    static AudiereSoundManager *Create(SexyAppBase *app);
    static void Init();
    static void Shutdown();
};

// Global accessor
AudiereSoundManager *GetAudiereSoundManager();

} // namespace Sexy

#endif // PVZ_SEXYAPPFRAMEWORK_SOUND_AUDIERE_SOUND_DRIVER_H
