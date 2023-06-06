// #include "dsp.h"
#include "sine.h"
#include "Utility/parameter_interpolator.h"
#include <math.h>

using namespace daisysp;

// from stmlib/dsp/dsp.h

#define MAKE_INTEGRAL_FRACTIONAL(x) \
  int32_t x ## _integral = static_cast<int32_t>(x); \
  float x ## _fractional = x - static_cast<float>(x ## _integral);

float SineOscillator::Interpolate(const float* table, float index, float size) {
  index *= size;
  MAKE_INTEGRAL_FRACTIONAL(index)
  float a = table[index_integral];
  float b = table[index_integral + 1];
  return a + (b - a) * index_fractional;
}

float SineOscillator::InterpolateWrap(const float* table, float index, float size) {
  index -= static_cast<float>(static_cast<int32_t>(index));
  index *= size;
  MAKE_INTEGRAL_FRACTIONAL(index)
  float a = table[index_integral];
  float b = table[index_integral + 1];
  return a + (b - a) * index_fractional;
}


void SineOscillator::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    phase_ = 0.0f;
    frequency_ = 0.0f;
    amplitude_ = 0.0f;
}

// from eurorack/plaits/dsp/oscillator/sine_oscillator.h

// Safe for phase >= 0.0f, will wrap.
float SineOscillator::Sine(float phase) {
    return InterpolateWrap(lut_sine, phase, kSineLUTSize);
}

// Potentially unsafe, if phase >= 1.25.
float SineOscillator::SineNoWrap(float phase) {
    return Interpolate(lut_sine, phase, kSineLUTSize);
}

// With positive of negative phase modulation up to an index of 32.
float SineOscillator::SinePM(uint32_t phase, float pm) {
    const float max_uint32 = 4294967296.0f;
    const int max_index = 32;
    const float offset = float(max_index);
    const float scale = max_uint32 / float(max_index * 2);

    phase += static_cast<uint32_t>((pm + offset) * scale) * max_index * 2;
    
    uint32_t integral = phase >> (32 - kSineLUTBits);
    float fractional = static_cast<float>(phase << kSineLUTBits) / float(max_uint32);
    float a = lut_sine[integral];
    float b = lut_sine[integral + 1];
    return a + (b - a) * fractional;
}

// Direct lookup without interpolation.
float SineOscillator::SineRaw(uint32_t phase) {
  return lut_sine[phase >> (32 - kSineLUTBits)];
}

void SineOscillator::Next(float frequency, float amplitude, float* sin, float* cos) {
    if (frequency >= 0.5f) {
        frequency = 0.5f;
    }

    phase_ += frequency;
    if (phase_ >= 1.0f) {
        phase_ -= 1.0f;
    }

    *sin = amplitude * SineNoWrap(phase_);
    *cos = amplitude * SineNoWrap(phase_ + 0.25f);
}

void SineOscillator::Render(float frequency, float amplitude, float* out, size_t size) {
    RenderInternal<true>(frequency, amplitude, out, size);
}

void SineOscillator::Render(float frequency, float* out, size_t size) {
    RenderInternal<false>(frequency, 1.0f, out, size);
}


void SineOscillator::SetFreq(float freq)
{
    if (freq >= 0.5f) {
      freq = 0.5f;
    }

    frequency_ = freq;
}

void SineOscillator::SetPhase(float phase)
{
    if (phase < 0.0f) {
      phase = 0.0f;
    }

    if (phase > 1.0f) {
      phase = 1.0f;
    } 

    phase_ = phase;
}

void SineOscillator::SetAmp(float amp)
{
    amplitude_ = amp;
}

template<bool additive>
void SineOscillator::RenderInternal(float frequency, float amplitude, float* out, size_t size) {
    if (frequency >= 0.5f) {
        frequency = 0.5f;
    }

    ParameterInterpolator fm(&frequency_, frequency, size);
    ParameterInterpolator am(&amplitude_, amplitude, size);

    while (size--) {
        phase_ += fm.Next();
        if (phase_ >= 1.0f) {
            phase_ -= 1.0f;
        }
        float s = SineNoWrap(phase_);
        if (additive) {
            *out++ += am.Next() * s;
        } else {
            *out++ = s;
        }
    }
} 