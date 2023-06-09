#pragma once
#ifndef FM_2OP_H
#define FM_2OP_H

#include "Synthesis/sine.h"
#include "Control/phasor.h"
#include "Utility/dsp.h"
#include <stdint.h>
#include <stdlib.h>
#ifdef __cplusplus

/** @file fm_2op.h */

namespace daisysp
{
/**  
    @brief 2op FM Module \n 
    @author Making Sound Machines
    @date June 2023 \n
    A module using phasors + LUT to make FM voices.
    Inspired by "Let's learn some FM" - Ess Mattison / fors.fm. \n
    https://www.twitch.tv/videos/573170445 \n
*/



class FM_2op
{
  public:
    FM_2op() {}
    ~FM_2op() {}


    /** Init Skew Oscillator module
        \param sample_rate Audio engine sample rate.
    */
    void Init(float sample_rate);

    /** Process
        Returns one sample at sample rate.
    */
    float Process();

    /** From Musicdsp.org "Fast power and root estimates for 32bit floats)
    Original code by Stefan Stenzel
    These are approximations
    */
    float fastpower(float f, int n);

    /** same same but different
    */
    float pow4(float x);

    /** Voice
    */
    float Voice(float freq, float ratio, float fine, float phase_mod, 
        float ph, float noise_level, float fb, float fb_mod, bool trig,
        float mod_level, float amp); 

    /** Operator
    */
    float Operator(Phasor& phasor_, float *fbk, float freq, float ratio, float fine, float phase_mod, 
        float ph, float noise_level, float fb, bool trig);   

    /** Set the frequency
        \param freq Frequency in Hz.
    */
    void SetFreq(float freq);

    /** Set the mod level +/- 1.0f
        \param freq Frequency in Hz.
    */
    void SetModLevel(float mod_level);

    /** Set the mod level +/- 1.0f
        \param mod_ratio
    */
    void SetModRatio(float mod_ratio);

    /** Set the phase
        \param phase 0.0f - 1.0f
    */
    void SetPhase(float phase);

    /** Set the phase
        \param phase 0.0f - 1.0f
    */
    inline void ResetPhase()
    {
        phase_ = 0.0f;
    }

    /** Set skew
        \param feedback +/-1.0f, clamped to to +/-0.99f
    */
    void SetFeedback(float Feedback);

    /** Set the Amplitude
        \param amp positive or negative float
    */
    void SetAmp(float amp);

    /** Returns true if cycle is at end of cycle. Set during call to Process.
    */
    inline bool IsEOC() { return eoc_; }

    /** Returns true if cycle is at end of cycle. Set during call to Process.
    */
    inline float GetPhase() { return phase_; } 

  private:
    Phasor phasor_op0_;
    Phasor phasor_op1_;
    Phasor phasor_op2_;
    SineOscillator sine_;

    uint8_t wave_;
    float sample_rate_, sr_recip_;

    // Oscillator state.
    float frequency_;
    float ratio_;
    float finetune_;

    float phase_;
    float phase_inc_; 
    float phase_mod_;
    float phase_offset_;

    float skew_;
    float noise_level_; 
    float feedback_;
    
    float amplitude_;
    bool trigger_;

    float mod_level_;

    float fb_op2_;
    float fb_op1_;
    float fb_op0_;
    float feedback_mod_;

    float last_out_;
    float eoc_;    
};

} // namespace daisysp
#endif
#endif