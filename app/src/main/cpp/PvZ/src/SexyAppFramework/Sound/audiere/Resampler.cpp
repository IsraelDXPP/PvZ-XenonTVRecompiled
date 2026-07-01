#include "PvZ/SexyAppFramework/Sound/audiere/AudioTypes.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdlib>

namespace audiere {

static void dumb_reset_resampler(Resampler::DumbResampler *r, int16_t *src, int srcSamples,
                                 int channels, int sampleRate) {
    r->field3 = sampleRate;
    r->field1 = reinterpret_cast<int32_t>(src);
    r->field0 = 0;
    r->field4 = channels;
    r->field13 = -1;
    r->field5 = 1;
    r->field12 = 0;
    r->field9 = 2;
    r->field2 = 0;
    r->field6 = 0.0f;
    r->field7 = 0.0f;
    r->field8 = 0;
    r->field10 = 0;
    r->field11 = 0;
}

static void dumb_end_resampler(Resampler::DumbResampler *r) {
    if (r) free(r);
}

static Resampler::DumbResampler *dumb_start_resampler(int16_t *src, int srcSamples,
                                                       int channels, int sampleRate) {
    auto *r = static_cast<Resampler::DumbResampler *>(malloc(sizeof(Resampler::DumbResampler)));
    if (r) dumb_reset_resampler(r, src, srcSamples, channels, sampleRate);
    return r;
}

static int dumb_resample_get_channel_sample(Resampler::DumbResampler *r, float pos, int channel) {
    int ipos = static_cast<int>(pos);
    int channels = r->field4;
    int srcSamples = r->field3;
    auto *src = reinterpret_cast<int16_t *>(r->field1);
    if (ipos < 0) ipos = 0;
    if (ipos >= srcSamples) ipos = srcSamples - 1;
    float frac = pos - ipos;
    int idx = ipos * channels + channel;
    if (idx + 1 > srcSamples * channels) return 0;
    int16_t s0 = src[idx];
    int16_t s1 = (ipos + 1 < srcSamples) ? src[idx + channels] : s0;
    return static_cast<int>((s0 + (s1 - s0) * frac));
}

static int dumb_resample_get_current_sample(Resampler::DumbResampler *r, float pos) {
    return dumb_resample_get_channel_sample(r, pos, 0);
}

static int dumb_resample5(Resampler::DumbResampler *r, int16_t *out, int outSamples,
                          float ratio, float a5) {
    int channels = r->field4;
    float pos = r->field6;
    int produced = 0;
    for (int i = 0; i < outSamples; i++) {
        if (channels == 2) {
            int sampleL = dumb_resample_get_channel_sample(r, pos, 0);
            int sampleR = dumb_resample_get_channel_sample(r, pos, 1);
            out[produced++] = static_cast<int16_t>(sampleL);
            out[produced++] = static_cast<int16_t>(sampleR);
        } else {
            int sample = dumb_resample_get_channel_sample(r, pos, 0);
            out[produced++] = static_cast<int16_t>(sample);
        }
        pos += ratio;
    }
    r->field6 = pos;
    r->field7 += a5;
    return produced;
}

static int dumb_resample4(Resampler::DumbResampler *r, int16_t *out, int outSamples, float ratio) {
    return dumb_resample5(r, out, outSamples, ratio, 0.0f);
}

Resampler::Resampler(SampleSource *source, int sampleRate)
    : mSource(source)
    , mSampleRate(sampleRate)
    , mResampler(nullptr)
    , mPitchShift(1.0f)
    , mRefCount(1)
{
    if (source) source->ref();
    // resampler will be created on first read
}

Resampler::~Resampler() {
    if (mResampler) {
        dumb_end_resampler(mResampler);
        mResampler = nullptr;
    }
    if (mSource) {
        mSource->unref();
        mSource = nullptr;
    }
}

void Resampler::ref() { mRefCount++; }
void Resampler::unref() { if (--mRefCount <= 0) delete this; }

int Resampler::read(int samples, void *buffer) {
    if (!mSource || !buffer) return 0;
    if (mPitchShift == 1.0f) {
        return mSource->read(samples, buffer);
    }
    if (mPitchShift <= 0.0f) return 0;
    int srcSampleRate = mSource->getSampleRate();
    if (srcSampleRate <= 0) return 0;
    float ratio = mPitchShift;
    int inputSamples = static_cast<int>(samples * ratio) + 16;
    // Allocate temp buffer for source samples
    auto *tmpBuf = static_cast<int16_t *>(malloc(inputSamples * sizeof(int16_t) * 2));
    if (!tmpBuf) return 0;
    int srcRead = mSource->read(inputSamples, tmpBuf);
    if (srcRead <= 0) {
        free(tmpBuf);
        return 0;
    }
    // Initialize resampler if needed
    if (!mResampler) {
        mResampler = dumb_start_resampler(tmpBuf, srcRead,
                                          mSource->getChannels(), srcSampleRate);
    } else {
        dumb_reset_resampler(mResampler, tmpBuf, srcRead,
                             mSource->getChannels(), srcSampleRate);
    }
    auto *out = static_cast<int16_t *>(buffer);
    int produced = dumb_resample4(mResampler, out, samples, ratio);
    free(tmpBuf);
    return produced;
}

void Resampler::reset() {
    if (mResampler) {
        dumb_end_resampler(mResampler);
        mResampler = nullptr;
    }
    if (mSource) mSource->reset();
}

int Resampler::getPosition() { return mSource ? mSource->getPosition() : 0; }
int Resampler::setPosition(int position) {
    reset();
    return mSource ? mSource->setPosition(position) : 0;
}

int Resampler::getLength() { return mSource ? mSource->getLength() : 0; }
bool Resampler::isSeekable() { return mSource ? mSource->isSeekable() : false; }
int Resampler::getSampleRate() { return mSampleRate; }
int Resampler::getChannels() { return mSource ? mSource->getChannels() : 0; }
int Resampler::getBitsPerSample() { return 16; }

void Resampler::setPitchShift(float shift) { mPitchShift = shift; }
float Resampler::getPitchShift() { return mPitchShift; }

int Resampler::fillBuffers() {
    return 0;
}

} // namespace audiere
