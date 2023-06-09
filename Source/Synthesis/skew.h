#pragma once
#ifndef SKEW_H
#define SKEW_H

#include "Synthesis/sine.h"
#include "Control/phasor.h"
#include "Utility/dsp.h"
#include <stdint.h>
#include <stdlib.h>
#ifdef __cplusplus

/** @file skew.h */

namespace daisysp
{
/**  
    @brief     Skew Oscillator Module \n 
    @author Making Sound Machines
    @date June 2023 \n
    A module using phasors + LUT to skew and tilt waveforms.
    Inspired by "Let's learn some FM" - Ess Mattison / fors.fm. \n
    https://www.twitch.tv/videos/573170445 \n
*/



class Skew
{
  public:
    Skew() {}
    ~Skew() {}

    /** Waveshapes
    */
    enum
    {
        SKEW_SINE,
        SKEW_SINE_PM,
        SKEW_SINE_SIG,
        SKEW_SINE_SIG_PM,        
        SKEW_SINE_SQUARE,
        SKEW_TRI_SQUARE,
        SKEW_TRI_SAW,
        SKEW_SIGMOID,
        SKEW_SQUARE_PWM_POLYBLEP,
        SKEW_TRI_PWM_POLYBLEP,
        SKEW_SHARK_PWM_POLYBLEP,
        SKEW_SQUARE_PWM,
        SKEW_LAST
    };


    /** Init Skew Oscillator module
        \param sample_rate Audio engine sample rate.
    */
    void Init(float sample_rate);

    /** Process
        Returns one sample at sample rate.
    */
    float Process();

    /** Normalized tunable sigmoid by Dale H. Emery
        \return https://dhemery.github.io/DHE-Modules/technical/sigmoid/
        for k -1.0f to 1.0f
    */
    float Sigmoid(float x, float k);

    /** tanh() approx from JUCE
    */
    float tanh_approx(float x);

    /** Plain Sine
    */
    float Sine(float phase);

    /** Plain Sine with skew = phase mod +/- 32.f (
     * one phase shift is 0.0f - 1.0f
    */
    float SinePM(float phase, float skew);

    /** Skewed Sine
    */
    float SineSigmoid(float phase, float skew);

    /** Skewed Sine optimized for phase mod
    */
    float SineSigmoidPM(float phase, float skew);

    /** Sine which can get more squarish
    */
    float SineSquare(float phase, float skew);    

    /** Triangle which can get more squarish
    */
    float TriSquare(float phase, float skew); 

    /** Triangle to Ramp; safe for +/-1.0f
    */
    float TriSaw(float phase, float skew);   

    /** Sigmoid
    */
    float RampSigmoid(float phase, float skew);

    /** Helper for skewed band-limited PWM
    */
    float Saw_Polyblep(float phase, float phase_inc);

    /** Skewed Polyblep Square
    */
    float SquarePWM_Polyblep(float phase, float skew, float phase_inc);

    /** Skewed band-limited Triangle 
    */
    float TrianglePWM_Polyblep(float phase, float skew, float phase_inc);

    /** Skewed Polyblep Square
    */
    float Shark_Polyblep(float phase, float skew, float phase_inc);

    /** Skewed Square - PWM
    not bandlimited, for LFO - use Polyblep version for audio
    */
    float SquarePWM(float phase, float skew);

    /** Polyblep
    */
    static float Polyblep(float phase_inc, float t);

    /** Sets the waveform to be synthesized by the Process() function.
    */
    inline void SetWaveform(const uint8_t waveform)
    {
        wave_ = waveform < SKEW_LAST ? waveform : SKEW_SINE;
    }    

    /** Set the frequency
        \param freq Frequency in Hz.
    */
    void SetFreq(float freq);

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
        \param skew +/-1.0f, clamped to to +/-0.99f
    */
    void SetSkew(float skew);

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
    Phasor phasor_;
    SineOscillator sine_;

    uint8_t wave_;
    float sample_rate_;

    // Oscillator state.
    float frequency_;
    float amplitude_;
    float phase_;
    float skew_;
    float phase_inc_;
    float last_out_;
    float eoc_;
};

} // namespace daisysp
#endif
#endif