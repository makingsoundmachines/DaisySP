#include <math.h>
#include "phasor.h"
#include "Utility/dsp.h"

using namespace daisysp;

constexpr float TWO_PI_RECIP = 1.0f / TWOPI_F;

void Phasor::SetFreq(float freq)
{
    freq_ = freq;
    //inc_  = (TWOPI_F * freq_) / sample_rate_;
    inc_  = (TWOPI_F * freq_) * sr_recip_;
}

float Phasor::Process()
{
    float out;
    //out = phase_ / TWOPI_F;
    out = phase_ * TWO_PI_RECIP;
    phase_ += inc_;
    eoc_ = false;

    if(phase_ > TWOPI_F)
    {
        phase_ -= TWOPI_F;
        eoc_ = true;
    }
    if(phase_ < 0.0f)
    {
        phase_ = 0.0f;
    }
    return out;
}
