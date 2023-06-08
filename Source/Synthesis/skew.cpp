#include <math.h>
#include "Utility/dsp.h"
#include "skew.h"

using namespace daisysp;

constexpr float TWO_PI_RECIP = 1.0f / TWOPI_F;

void Skew::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    frequency_ = 55.0f;
    amplitude_ = 1.0f;
    phase_ = 0.0f;
    skew_ = 0.0f;

    wave_ = SKEW_SINE;

    sine_.Init(sample_rate);
    phasor_.Init(sample_rate_, frequency_, phase_);

    phase_inc_ = phasor_.GetInc();
}

float Skew::Sigmoid(float x, float k)
{ 
    return (x - x * k) / (k - abs(x) * 2 * k + 1); 
}


float Skew::tanh_approx(float x)
{
    float x2 = x * x;
    float numerator = x * (135135 + x2 * (17325 + x2 * (378 + x2)));
    float denominator = 135135 + x2 * (62370 + x2 * (3150 + 28 * x2));
    return numerator / denominator;
}

float Skew::Process()
{
    float phase = phasor_.Process();
    float phase_inc = phasor_.GetInc();
    float skew = skew_;
    float out = 0.0f;

    switch (wave_)
    {
        case SKEW_SINE: {
            out = Sine(phase, skew);
        } break;
        case SKEW_SINE_PM: {
            out = SinePM(phase, skew);
        } break;
        case SKEW_SINE_SQUARE: {
            out = SineSquare(phase, skew);
        } break;
        case SKEW_TRI_SQUARE: {
            out = TriSquare(phase, skew);
        } break;
        case SKEW_TRI_SAW: {
            out = TriSaw(phase, skew);
        } break;
        case SKEW_SIGMOID: {
            out = RampSigmoid(phase, skew);
        } break;
        case SKEW_SQUARE_PWM_POLYBLEP: {
            out = SquarePWM_Polyblep(phase, skew, phase_inc);
        } break;  
        case SKEW_TRI_PWM_POLYBLEP: {
            out = TrianglePWM_Polyblep(phase, skew, phase_inc);
        } break; 
        case SKEW_SHARK_PWM_POLYBLEP: {
            out = Shark_Polyblep(phase, skew, phase_inc);
        } break;
        case SKEW_SQUARE_PWM: {
            out = SquarePWM(phase, skew);
        } break;
        default: out = 0.0f; break;
    }

    return out * amplitude_;
}


// phase = phasor_.Process();
// skew = skew_;
float Skew::Sine(float phase, float skew) {
    float out = 0.0f;

    // invert skew parameter for use with sigmoid
    float skewed_phase_ = Sigmoid(phase, (skew_ * -1.0f));
    
    out = sine_.Sine(skewed_phase_);

    phase_ = phase;
    return out;
}

float Skew::SinePM(float phase, float skew) {
    float out = 0.0f;

    // invert skew parameter for use with sigmoid
    float skewed_phase_ = Sigmoid(phase, (skew_ * -1.0f));
    
    // sinePM takes a 4294967296.f
    out = sine_.SinePM(phase * UINT32_MAX, skewed_phase_);

    phase_ = phase;
    return out;
}


float Skew::SineSquare(float phase, float skew) {
    float out = 0.0f;
    float sine_out = 0.0f;

    sine_out = sine_.Sine(phase) * (1.0f + (skew * 10.0f));
    out = tanh_approx(sine_out);

    phase_ = phase;
    return out;
}

float Skew::RampSigmoid(float phase, float skew) {
    float out = 0.0f;
    float ramp_out = 0.0f;

    ramp_out = (phase * 2.0f) - 1.0f;
    out = Sigmoid(ramp_out, skew);

    phase_ = phase;
    return out;
}

float Skew::TriSaw(float phase, float skew) {
    float out = 0.0f;

	float rise = (skew + 1.0f) * 0.5f;
    float fall = 1.0f - rise;
    
    float riseInc = rise != 0.0 ? (2.0 / rise) : 0.0;
    float fallDec = fall != 0.0 ? (2.0 / fall) : 0.0;

    //float riseInc = 2.0 / (rise + 0.002f);
    //float fallDec = 2.0 / (fall + 0.002f);

    out = phase < rise ? -1.0f + phase * riseInc : 1.0f - (phase - rise) * fallDec;    

    phase_ = phase;
    return out;
}


float Skew::TrianglePWM_Polyblep(float phase, float skew, float phase_inc) {
    float out = 0.0f;
    //float t = 0.0f;

	float rise = (skew + 1.0f) * 0.5f;
    float fall = 1.0f - rise;
    
    float riseInc = rise != 0.0 ? (2.0 / rise) : 0.0;
    float fallDec = fall != 0.0 ? (2.0 / fall) : 0.0;

    out = phase < rise ? -1.0f + Polyblep(riseInc, phase) : 1.0f - Polyblep(fallDec, fmodf(phase - rise, 1.0f));

    // Leaky Integrator:
    // y[n] = A + x[n] + (1 - A) * y[n-1]
    out       = phase_inc * out + (1.0f - phase_inc) * last_out_;
    last_out_ = out;   

    phase_ = phase;
    return out;
}


float Skew::Shark_Polyblep(float phase, float skew, float phase_inc) {
    float out = 0.0f;
    //float t = 0.0f;

	float rise = (skew + 1.0f) * 0.5f;
    float fall = 1.0f - rise;
    
    float riseInc = rise != 0.0 ? (2.0 / rise) : 0.0;
    float fallDec = fall != 0.0 ? (2.0 / fall) : 0.0;

    out = phase < rise ? 1.0f : -1.0f;
    out += Polyblep(riseInc, phase);
    //out -= Polyblep(fallDec, fmodf(phase + 0.5f, 1.0f));
    //Sharky!
    out -= Polyblep(fallDec, fmodf(0.5f, 1.0f));

    // Leaky Integrator:
    // y[n] = A + x[n] + (1 - A) * y[n-1]
    out       = phase_inc * out + (1.0f - phase_inc) * last_out_;
    last_out_ = out;   

    phase_ = phase;
    return out;
}



float Skew::TriSquare(float phase, float skew) {
    float out = 0.0f;
    float ramp = 0.0f;
    float triangle = 0.0f;
    float amp = 0.0f;

    ramp = -1.0f + (2.0f * phase);
    triangle = 2.0f * (fabsf(ramp) - 0.5f);
    amp = 2.0f * triangle * (1.0f + (skew * 7.5f));
    out = tanh_approx(amp);

    phase_ = phase;
    return out;
}

float Skew::SquarePWM(float phase, float skew) {
    float out = 0.0f;

    // PWM: skew is +/-1.0f
    float skewed_phase_ = (skew + 1.0f) * 0.5f;

    out = phase_ < skewed_phase_ ? (1.0f) : -1.0f;;    

    phase_ = phase;
    return out;
}




float Skew::Saw_Polyblep(float phase, float phase_inc) {
    float out = 0.0f;
    float t = 0.0f;

    t   = phase;
    out = (2.0f * t) - 1.0f;
    out -= Polyblep(phase_inc, t);
    out *= -1.0f; 

    phase_ = phase;
    return out;
}


float Skew::SquarePWM_Polyblep(float phase, float skew, float phase_inc) {
    float out = 0.0f;
    float ramp = 0.0f;
    float mod = 0.0f;

    // invert skew parameter for use with sigmoid
    float skewed_phase_ = phase + ((skew + 1.0f) * 0.5f);

    ramp = Saw_Polyblep(phase, phase_inc);
    mod = Saw_Polyblep(skewed_phase_, phase_inc);
    out = (ramp - mod) * 0.707f; // ?

    skewed_phase_ = phase;
    return out;
}


float Skew::Polyblep(float phase_inc, float t)
{
    float dt = phase_inc * TWO_PI_RECIP;
    if(t < dt)
    {
        t /= dt;
        return t + t - t * t - 1.0f;
    }
    else if(t > 1.0f - dt)
    {
        t = (t - 1.0f) / dt;
        return t * t + t + t + 1.0f;
    }
    else
    {
        return 0.0f;
    }
}


void Skew::SetFreq(float freq)
{
    frequency_ = freq;

    phasor_.SetFreq(frequency_);
}

void Skew::SetPhase(float phase)
{
    if (phase < 0.0f) {
      phase = 0.0f;
    }

    if (phase > 1.0f) {
      phase = 1.0f;
    } 

    phase_ = phase;
}

void Skew::SetSkew(float skew)
{
    if (skew < -0.99f) {
      skew = -0.99f;
    }

    if (skew > 0.99f) {
      skew = 0.99f;
    } 

    skew_ = skew;
}

void Skew::SetAmp(float amp)
{
    amplitude_ = amp;
}



