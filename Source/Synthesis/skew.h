#pragma once
#ifndef SKEW_H
#define SKEW_H

#include "Synthesis/sine.h"
#include "Control/phasor.h"
#include "Control/fm_utils.h"
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
    Inspired by Ess Mattison / fors.fm. \n
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


    /** Init ZOscillator module
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

    /** Skewed Sine
    */
    float Sine(float phase, float skew);

    /** Skewed Sine optimized for phase mod
    */
    float SinePM(float phase, float skew);

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

    /** Set skew
        \param skew +/-1.0f, clamped to to +/-0.99f
    */
    void SetSkew(float skew);

    /** Set the Amplitude
        \param amp positive or negative float
    */
    void SetAmp(float amp);

  private:
    Phasor phasor_;
    SineOscillator sine_;
    FM_utility sigmoid_;

    uint8_t wave_;
    float sample_rate_;

    // Oscillator state.
    float frequency_;
    float amplitude_;
    float phase_;
    float skew_;
    float phase_inc_;
    float last_out_;
};

} // namespace daisysp
#endif
#endif