#include <math.h>
#include "Utility/dsp.h"
#include "fm_2op.h"

using namespace daisysp;

constexpr float TWO_PI_RECIP = 1.0f / TWOPI_F;

float FM_2op::fastpower(float f, int n)
{
    long *lp, l;
    lp = (long *)(&f);
    l  = *lp;
    l -= 0x3F800000;
    l <<= (n - 1);
    l += 0x3F800000;
    *lp = l;
    return f;
}

void FM_2op::Init(float sample_rate)
{
    sample_rate_ = sample_rate;
    sr_recip_ = 1.0f / sample_rate;

    frequency_ = 55.0f;
    ratio_ = 2.0f;
    finetune_ = 0.0f;

    phase_ = 0.0f;
    phase_mod_ = 0.0f;
    phase_offset_ = 0.0f;
    
    fb_op2_ = 0.0f;
    fb_op1_ = 0.0f;
    fb_op0_ = 0.0f;

    feedback_ = 0.0f;
    feedback_mod_ = 1.0f;
    last_out_ = 0.0f;
    
    mod_level_ = 0.0f;
    amplitude_ = 1.0f;
    trigger_ = false;

    float freq = (frequency_ * ratio_) + finetune_;

    phasor_op0_.Init(sample_rate_, freq, phase_);
    phasor_op1_.Init(sample_rate_, freq, phase_);
    phasor_op2_.Init(sample_rate_, frequency_, phase_);
    
    sine_.Init(sample_rate);

    phase_inc_ = phasor_op0_.GetInc();
    eoc_ = phasor_op0_.IsEOC();
}

// Test one op
/*
float FM_2op::Process()
{    
    float out = 0.0f;

    float freq = frequency_; 
    float ratio = ratio_; 
    float fine = finetune_;

    float phase_mod = phase_mod_;
    float ph = phase_offset_;
    float noise_level = noise_level_; 
    float fb = feedback_;
    //float fb_mod = feedback_mod_;

    float amp = amplitude_;
    bool trig = trigger_;

    out = Operator(phasor_op0_, &fb_op0_, freq, ratio, fine, phase_mod, ph, noise_level, fb, trig);

    return out * amp;
}
*/


float FM_2op::Process()
{    
    float out = 0.0f;

    float freq = frequency_; 
    float ratio = ratio_; 
    float fine = finetune_;

    float phase_mod = phase_mod_;
    float ph = phase_offset_;
    float noise_level = noise_level_; 
    float fb = feedback_;
    float fb_mod = feedback_mod_;
    
    float mod_level = mod_level_;

    float amp = amplitude_;
    bool trig = trigger_;

    out = Voice(freq, ratio, fine, phase_mod, ph, noise_level, fb, fb_mod, trig, mod_level, amp);

    return out * amp;
}


float FM_2op::pow4(float x) {

    x *= x;
    x *= x;

    return x;
}


float FM_2op::Voice(float freq, float ratio, float fine, float phase_mod, 
        float ph, float noise_level, float fb, float fb_mod, bool trig,
        float mod_level, float amp) {

    float out = 0.0f;
    
    float op2 = 0.0f;
    float op1 = 0.0f;
    float op0 = 0.0f;

    float op2_ph = 0.5f + ph;
    float op1_ph = 0.5f + ph;

    // to be replaced with an env
    float fm_env = 1.0f;

    // to be replaced with an env
    float noise_env   = noise_level * 0.0f;
    float noise_scale = noise_level * 0.0f;

    // to be replaced with an env
    float fb_mod_   = fb * 1.0f;

    //    Parameters:   phasor, feedback, freq, ratio, fine, phase_mod,   ph,      noise,      fb, trig

	op2 = Operator(phasor_op2_, &fb_op2_, freq, ratio, fine,    0.0f, op2_ph,  noise_env, fb_mod_, trig) * fm_env;

    float op2_lvl = op2 * (mod_level * 0.5f);

	op1 = Operator(phasor_op1_, &fb_op1_, freq, ratio, fine, op2_lvl, op1_ph,  noise_env, fb_mod_, trig) * fm_env;

    float op1_lvl = op1 * mod_level;

	op0 = Operator(phasor_op0_, &fb_op0_, freq,  1.0f, 0.0f, op1_lvl,  0.25f, noise_scale,   0.0f, trig);

    out = op0;

    return out * amp;
}

// phase = phasor_.Process();
// skew = skew_;
// Pass in a phasor and a float to store the last sample for feedback
float FM_2op::Operator(Phasor& phasor_, float *fbk, float freq, float ratio, float fine, 
    float phase_mod, float ph, float noise_level, float fb, bool trig) {
    
    // output
    float out = 0.0f;
    float last_out_ = *fbk;

    // the actual frequency: base frequency * ratio
    float frequency = freq * ratio + fine;
    phasor_.SetFreq(frequency);
	
    // n_ = 0.0 at Nyquist
    //float n = frequency * sr_recip_;
	//float n_ = 20.0f * pow(0.5f - n, 4.0f);

    float n = 0.5f - (frequency * sr_recip_);
	float n_ = 20.0f * pow4(n);

    // Reset phase on trigger
    if(trig) {
        last_out_ = 0.0f;
        out = 0.0f;
        phasor_.Reset();
    }

    //float mod = phase_mod + n_;

    //phasor 
    float phase = phasor_.Process();

    //float mod = phase_mod + n_;
    //phase offset + feedback + modulation
	float mod = ph + (last_out_ * fb) + (phase_mod * n_);

    out = sine_.SinePM(phase * UINT32_MAX, mod);
    // out = sine_.SinePM(phase);    
	
	if (fb > 0.0f) {
		*fbk = ((out * fb * n_) + last_out_) * 0.5f;
	}
	if (fb < 0.0f) {
		*fbk = ((out * out * fb * n_) + last_out_) * 0.5f;
	}

    return out;
}

void FM_2op::SetFreq(float freq)
{
    frequency_ = freq;
}

void FM_2op::SetModLevel(float mod_level)
{
    mod_level_ = mod_level;
}

void FM_2op::SetPhase(float phase)
{
    if (phase < 0.0f) {
      phase = 0.0f;
    }

    if (phase > 1.0f) {
      phase = 1.0f;
    } 

    phase_offset_ = phase;
}

void FM_2op::SetModRatio(float mod_ratio)
{
    if (mod_ratio < 0.0f) {
      mod_ratio = 0.0f;
    }

    ratio_ = mod_ratio;
}

void FM_2op::SetFeedback(float feedback)
{
    if (feedback < -0.99f) {
      feedback = -0.99f;
    }

    if (feedback > 0.99f) {
      feedback = 0.99f;
    } 

    feedback_ = feedback;
}

void FM_2op::SetAmp(float amp)
{
    amplitude_ = amp;
}



