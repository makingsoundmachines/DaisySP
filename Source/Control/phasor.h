#pragma once
#ifndef DSY_PHASOR_H
#define DSY_PHASOR_H
#ifdef __cplusplus

#include "Utility/dsp.h"

namespace daisysp
{
/** Generates a normalized signal moving from 0-1 at the specified frequency.

\todo Selecting which channels should be initialized/included in the sequence conversion.
\todo Setup a similar start function for an external mux, but that seems outside the scope of this file.

*/
class Phasor
{
  public:
    Phasor() {}
    ~Phasor() {}
    /** Initializes the Phasor module
    sample rate, and freq are in Hz
    initial phase is in radians
    Additional Init functions have defaults when arg is not specified:
    - phase = 0.0f
    - freq = 1.0f
    */
    inline void Init(float sample_rate, float freq, float initial_phase)
    {
        sample_rate_ = sample_rate;
        phase_       = initial_phase;
        sr_recip_    = 1.0f / sample_rate;
        SetFreq(freq);
    }

    /** Initialize phasor with samplerate and freq
    */
    inline void Init(float sample_rate, float freq)
    {
        Init(sample_rate, freq, 0.0f);
    }

    /** Initialize phasor with samplerate
    */
    inline void Init(float sample_rate) { Init(sample_rate, 1.0f, 0.0f); }
    /** processes Phasor and returns current value
    */
    float Process();


    /** Sets frequency of the Phasor in Hz
    */
    void SetFreq(float freq);


    /** Returns current frequency value in Hz
    */
    inline float GetFreq() { return freq_; }


    /** Returns current phase increase
    */
    inline float GetInc() { return inc_; }


    /** Returns true if cycle is at end of cycle. Set during call to Process.
    */
    inline bool IsEOC() { return eoc_; } 

    /** Adds a value 0.0-1.0 (mapped to 0.0-TWO_PI) to the current phase. Useful for PM and "FM" synthesis.
    */
    inline void PhaseAdd(float phase) { phase_ += (phase * TWOPI_F); }

    /** Resets the phase to the input argument. If no argument is present, it will reset phase to 0.0;
    */
    inline void Reset(float phase = 0.0f) { phase_ = (phase * TWOPI_F); }       

  private:
    float freq_;
    float sample_rate_, sr_recip_, inc_, phase_, eoc_;
};
} // namespace daisysp
#endif
#endif
