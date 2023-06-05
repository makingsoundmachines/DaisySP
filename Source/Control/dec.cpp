#include "dec.h"
#include <math.h>

using namespace daisysp;


void Dec::Init(float sample_rate, int blockSize)
{
    sample_rate_  = sample_rate / blockSize;
    decayTime_  = -1.f;
    x_            = 0.0f;
    mode_         = DEC_SEG_IDLE;
    SetTime(DEC_SEG_DECAY, 0.1f);
}

void Dec::Trigger()
{
    x_ = 1.f;
    mode_    = DEC_SEG_DECAY;
}

void Dec::SetTime(int seg, float time)
{
    switch(seg)
    {
        case DEC_SEG_DECAY:
        {
            SetTimeConstant(time, decayTime_, decayD0_);
        }
        break;
        default: return;
    }
}

void Dec::SetDecayTime(float timeInS)
{
    SetTimeConstant(timeInS, decayTime_, decayD0_);
}


void Dec::SetTimeConstant(float timeInS, float& time, float& coeff)
{
    if(timeInS != time)
    {
        time = timeInS;
        if(time > 0.f)
        {
            const float target = logf(1. / M_E);
            coeff              = 1.f - expf(target / (time * sample_rate_));
        }
        else
            coeff = 1.f; // instant change
    }
}


float Dec::Process()
{
    float out = 0.0f;

    float D0;
    D0 = decayD0_;

    float target = -0.01f;
    switch(mode_)
    {
        case DEC_SEG_IDLE: out = 0.0f; break;
        case DEC_SEG_DECAY:
            x_ += D0 * (target - x_);
            out = x_;
            if(out < 0.0f)
            {
                x_ = out = 0.f;
                mode_    = DEC_SEG_IDLE;
            }
        default: break;
    }
    return out;
}
