#include "ade.h"
#include <math.h>

using namespace daisysp;


void Ade::Init(float sample_rate, int blockSize)
{
    sample_rate_  = sample_rate / blockSize;
    attackShape_  = -1.f;
    attackTarget_ = 0.0f;
    attackTime_   = -1.f;
    decayTime_    = -1.f;
    sus_level_    = 0.5f;
    x_            = 0.0f;
    mode_         = ADE_SEG_IDLE;

    SetTime(ADE_SEG_ATTACK, 0.1f);
    SetTime(ADE_SEG_DECAY, 0.1f);
}

void Ade::Trigger(bool hard)
{
    mode_ = ADE_SEG_ATTACK;
    if(hard)
        x_ = 0.f;
}

void Ade::SetTime(int seg, float time)
{
    switch(seg)
    {
        case ADE_SEG_ATTACK: SetAttackTime(time, 0.0f); break;
        case ADE_SEG_DECAY:
        {
            SetTimeConstant(time, decayTime_, decayD0_);
        }
        break;
        default: return;
    }
}

void Ade::SetAttackTime(float timeInS, float shape)
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

void Ade::SetDecayTime(float timeInS)
{
    SetTimeConstant(timeInS, decayTime_, decayD0_);
}


void Ade::SetTimeConstant(float timeInS, float& time, float& coeff)
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


float Ade::Process()
{
    float out = 0.0f;

    float D0(attackD0_);
    if(mode_ == ADE_SEG_DECAY)
        D0 = decayD0_;

    float target = sus_level_;
    switch(mode_)
    {
        case ADE_SEG_END: out = sus_level_; break;
        case ADE_SEG_IDLE: out = 0.0f; break;
        case ADE_SEG_ATTACK:
            x_ += D0 * (attackTarget_ - x_);
            out = x_;
            if(out > 1.f)
            {
                x_ = out = 1.f;
                mode_    = ADE_SEG_DECAY;
            }
            break;
        case ADE_SEG_DECAY:
            x_ += D0 * (target - x_);
            out = x_;
            if(out < sus_level_)
            {
                x_ = out = 0.f;
                mode_    = ADE_SEG_END;
            }
        default: break;
    }
    return out;
}
