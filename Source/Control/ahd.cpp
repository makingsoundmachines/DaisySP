#include "ahd.h"
#include <math.h>

using namespace daisysp;

float Ahd::Sigmoid(float x, float y) 
{ 
    return (x - x * y) / (y - abs(x) * 2 * y + 1); 
}


void Ahd::Init(float sample_rate, int blockSize)
{
    sample_rate_  = sample_rate / blockSize;
    attackShape_  = -1.f;
    attackTarget_ = 0.0f;
    attackTime_   = -1.f;
    decayTime_  = -1.f;
    x_            = 0.0f;
    mode_         = AHD_SEG_IDLE;

    SetTime(AHD_SEG_ATTACK, 0.1f);
    SetTime(AHD_SEG_ATTACK, 0.5f);
    SetTime(AHD_SEG_DECAY, 0.1f);
}

void Ahd::Trigger(bool hard)
{
    mode_ = AHD_SEG_ATTACK;
    t_ = 0;
    if(hard)
        x_ = 0.f;
}

void Ahd::SetTime(int seg, float time)
{
    switch(seg)
    {
        case AHD_SEG_ATTACK: SetAttackTime(time, 0.0f); break;
        break;
        case AHD_SEG_HOLD: 
        {
            holdTime_samples_ = (uint32_t)(time * sample_rate_);
        }
        break;
        case AHD_SEG_DECAY:
        {
            SetTimeConstant(time, decayTime_, decayD0_);
        }
        break;
        default: return;
    }
}

void Ahd::SetAttackTime(float timeInS, float shape)
{
    if((timeInS != attackTime_) || (shape != attackShape_))
    {
        attackTime_  = timeInS;
        attackShape_ = shape;
        if(timeInS > 0.f)
        {
            float x         = shape;
            float target    = 9.f * powf(x, 10.f) + 0.3f * x + 1.01f;
            attackTarget_   = target;
            float logTarget = logf(1.f - (1.f / target)); // -1 for decay
            attackD0_       = 1.f - expf(logTarget / (timeInS * sample_rate_));
        }
        else
            attackD0_ = 1.f; // instant change
    }
}

void Ahd::SetHoldTime(float timeInS)
{
    holdTime_samples_ = (uint32_t)(timeInS * sample_rate_);
}

void Ahd::SetDecayTime(float timeInS)
{
    SetTimeConstant(timeInS, decayTime_, decayD0_);
}


void Ahd::SetTimeConstant(float timeInS, float& time, float& coeff)
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


float Ahd::Process()
{
    float out = 0.0f;

    float D0(attackD0_);
    if(mode_ == AHD_SEG_DECAY)
        D0 = decayD0_;

    // float D0 = mode_ == AHD_SEG_ATTACK ? attackD0_ : decayD0_;    

    float target = -0.01f;
    switch(mode_)
    {
        case AHD_SEG_IDLE: out = 0.0f; break;
        case AHD_SEG_ATTACK:
            x_ += D0 * (attackTarget_ - x_);
            out = x_;
            if(out > 1.f)
            {
                x_ = out = 1.f;
                t_ = 0;
                mode_    = AHD_SEG_HOLD;
            }
            break;
        case AHD_SEG_HOLD:
            t_ += 1;
            out = 1.f;
            if(t_ > holdTime_samples_)
            {
                x_ = out = 1.f;
                mode_    = AHD_SEG_DECAY;
            }
            break;
        case AHD_SEG_DECAY:
            x_ += D0 * (target - x_);
            out = x_;
            if(out < 0.0f)
            {
                x_ = out = 0.f;
                mode_    = AHD_SEG_IDLE;
            }
        default: break;
    }
    return out;
}
