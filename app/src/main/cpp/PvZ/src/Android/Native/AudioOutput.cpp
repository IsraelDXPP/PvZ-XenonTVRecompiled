#include "PvZ/Android/Native/AudioOutput.h"
#include "PvZ/SexyAppFramework/Sound/audiere/AudioTypes.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <cstdlib>
#include <cstring>

namespace audiere { extern AndroidAudioDevice *gActiveDevice; }

namespace Native {

// OpenSL ES engine globals
static SLObjectItf engineObject = nullptr;
static SLEngineItf engineEngine = nullptr;
static SLObjectItf outputMixObject = nullptr;
static SLObjectItf playerObject = nullptr;
static SLPlayItf playerPlay = nullptr;
static SLAndroidSimpleBufferQueueItf playerBufferQueue = nullptr;

// Audio buffer queue (double buffered)
static const int NUM_BUFFERS = 2;
static const int BUFFER_FRAMES = 2048;
static int16_t audioBuffers[NUM_BUFFERS][BUFFER_FRAMES * 2];
static int currentBuffer = 0;
static volatile bool isPlaying = false;
static volatile bool audioShutdown = false;

static void bufferQueueCallback(SLAndroidSimpleBufferQueueItf caller, void *context) {
    if (audioShutdown) return;
    if (audiere::gActiveDevice) {
        audiere::gActiveDevice->writeData();
    }
}

bool AudioOutput::setup(int sampleRate, int channels, int bits) {
    SLresult result;

    // Create engine
    result = slCreateEngine(&engineObject, 0, nullptr, 0, nullptr, nullptr);
    if (result != SL_RESULT_SUCCESS) return false;

    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) return false;

    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (result != SL_RESULT_SUCCESS) return false;

    // Create output mix
    const SLInterfaceID ids[] = {SL_IID_VOLUME};
    const SLboolean req[] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    if (result != SL_RESULT_SUCCESS) return false;

    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) return false;

    // Configure audio source
    SLDataLocator_AndroidSimpleBufferQueue locBufQ = {
        SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
        NUM_BUFFERS
    };
    SLDataFormat_PCM formatPcm = {
        SL_DATAFORMAT_PCM,
        (SLuint32)channels,
        (SLuint32)sampleRate * 1000,
        SL_PCMSAMPLEFORMAT_FIXED_16,
        SL_PCMSAMPLEFORMAT_FIXED_16,
        channels == 2 ? SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT : SL_SPEAKER_FRONT_CENTER,
        SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource audioSrc = {&locBufQ, &formatPcm};

    // Configure audio sink
    SLDataLocator_OutputMix locOutMix = {
        SL_DATALOCATOR_OUTPUTMIX,
        outputMixObject
    };
    SLDataSink audioSink = {&locOutMix, nullptr};

    // Create audio player
    const SLInterfaceID playerIds[] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME};
    const SLboolean playerReq[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &playerObject,
                                                 &audioSrc, &audioSink,
                                                 2, playerIds, playerReq);
    if (result != SL_RESULT_SUCCESS) return false;

    result = (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) return false;

    result = (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playerPlay);
    if (result != SL_RESULT_SUCCESS) return false;

    result = (*playerObject)->GetInterface(playerObject, SL_IID_BUFFERQUEUE, &playerBufferQueue);
    if (result != SL_RESULT_SUCCESS) return false;

    result = (*playerBufferQueue)->RegisterCallback(playerBufferQueue, bufferQueueCallback, nullptr);
    if (result != SL_RESULT_SUCCESS) return false;

    // Start playback (initially silent buffers)
    audioShutdown = false;
    for (int i = 0; i < NUM_BUFFERS; i++) {
        std::memset(audioBuffers[i], 0, sizeof(audioBuffers[i]));
        result = (*playerBufferQueue)->Enqueue(playerBufferQueue,
                                               audioBuffers[i],
                                               sizeof(audioBuffers[i]));
        if (result != SL_RESULT_SUCCESS) break;
    }
    result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING);
    if (result != SL_RESULT_SUCCESS) return false;
    isPlaying = true;
    return true;
}

void AudioOutput::shutdown() {
    audioShutdown = true;
    isPlaying = false;
    if (playerPlay) {
        (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_STOPPED);
    }
    if (playerObject) {
        (*playerObject)->Destroy(playerObject);
        playerObject = nullptr;
        playerPlay = nullptr;
        playerBufferQueue = nullptr;
    }
    if (outputMixObject) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = nullptr;
    }
    if (engineObject) {
        (*engineObject)->Destroy(engineObject);
        engineObject = nullptr;
        engineEngine = nullptr;
    }
}

int AudioOutput::write(const void *data, int dataSize) {
    if (!isPlaying || !playerBufferQueue) return 0;
    // Wait for a free buffer slot
    SLAndroidSimpleBufferQueueState state;
    SLresult result;
    int retries = 0;
    do {
        result = (*playerBufferQueue)->GetState(playerBufferQueue, &state);
        if (retries++ > 100) return 0;
    } while (state.count >= NUM_BUFFERS && !audioShutdown);
    if (audioShutdown) return 0;
    int copySize = dataSize < (int)sizeof(audioBuffers[0]) ? dataSize : sizeof(audioBuffers[0]);
    int16_t *buf = audioBuffers[currentBuffer];
    // If the buffer is currently in queue, we can't write to it, so write to the other one
    // Actually, we know at least one slot is free (count < NUM_BUFFERS)
    std::memcpy(buf, data, copySize);
    if (copySize < (int)sizeof(audioBuffers[0])) {
        std::memset((uint8_t*)buf + copySize, 0, sizeof(audioBuffers[0]) - copySize);
    }
    // Don't enqueue again if buffer queue is full
    // Instead, we rely on the callback mechanism
    // For simplicity, we use a blocking approach with buffer counts
    result = (*playerBufferQueue)->Enqueue(playerBufferQueue, buf, sizeof(audioBuffers[0]));
    if (result != SL_RESULT_SUCCESS) return 0;
    currentBuffer = (currentBuffer + 1) % NUM_BUFFERS;
    return dataSize;
}

void AudioOutput::initialize() {
    // audio pumping is handled directly in bufferQueueCallback
}

} // namespace Native
