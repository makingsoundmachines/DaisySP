#include "dsp.h"
#include "sidnoise.h"

using namespace daisysp;
// static inline float Polyblep(float phase_inc, float t);

constexpr float TWO_PI_RECIP = 1.0f / TWOPI_F;

float SIDNoise::Process()
{
    phase_ += phase_inc_;
    
    if(phase_ > TWOPI_F)
    {
        phase_ -= TWOPI_F;
        eoc_ = true;

        noiseout = (bit(reg,22) << 7) |
            (bit(reg,20) << 6) |
            (bit(reg,16) << 5) |
            (bit(reg,13) << 4) |
            (bit(reg,11) << 3) |
            (bit(reg, 7) << 2) |
            (bit(reg, 4) << 1) |
            (bit(reg, 2) << 0);

        /* Save bits used to feed bit 0 */
        bit22= bit(reg,22);
        bit17= bit(reg,17);

        /* Shift 1 bit left */
        reg= reg << 1;

        /* Feed bit 0 */
        reg= reg | (bit22 ^ bit17); 

        out = s82f(noiseout);       
    }
    else
    {
        eoc_ = false;
    }
    eor_ = (phase_ - phase_inc_ < PI_F && phase_ >= PI_F);

    return out * amp_;
}

long SIDNoise::bit(long val, int8_t bitnr) 
{
  return (val & (1<<bitnr))? 1:0;
}

float SIDNoise::CalcPhaseInc(float f)
{
    return (TWOPI_F * f) * sr_recip_;
}
