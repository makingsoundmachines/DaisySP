#pragma once
#ifndef DSY_SIDNOISE_H
#define DSY_SIDNOISE_H
#include <stdint.h>
#include "../Utility/dsp.h"
#ifdef __cplusplus

namespace daisysp
{
/** Synthesis of SIDNoise
*/

#define S82F_SCALE 0.00390625f     /**< 1 / (2**8) */
#define F2S8_SCALE 255.0f          /**< (2 ** 8) - 1 */


class SIDNoise
{
  public:
    SIDNoise() {}
    ~SIDNoise() {}

    /** Initializes the Oscillator

        \param sample_rate - sample rate of the audio engine being run, and the frequency that the Process function will be called.

        Defaults:
        - freq_ = 100 Hz
        - amp_ = 0.5
        - waveform_ = sine wave.
    */
    void Init(float sample_rate)
    {
        sr_        = sample_rate;
        sr_recip_  = 1.0f / sample_rate;
        freq_      = 100.0f;
        amp_       = 0.5f;
        phase_     = 0.0f;
        phase_inc_ = CalcPhaseInc(freq_);
        eoc_       = true;
        eor_       = true;
    }


    /** Changes the frequency of the Noise, and recalculates phase increment.
    */
    inline void SetFreq(const float f)
    {
        freq_      = f;
        phase_inc_ = CalcPhaseInc(f);
    }


    /** Sets the amplitude of the waveform.
    */
    inline void SetAmp(const float a) { amp_ = a; }

    inline float s82f(int8_t x)
    {
        return (float)x * S82F_SCALE;
    }


    /** Processes the waveform to be generated, returning one sample. This should be called once per sample period.
    */
    float Process();

  private:
    float   CalcPhaseInc(float f);
    long    bit(long val, int8_t bitnr);
    float   amp_, freq_;
    float   sr_, sr_recip_, phase_, phase_inc_;
    float   last_out_, last_freq_;
    bool    eor_, eoc_;
    
    float   out = 0;
    int8_t  noiseout;

    long bit22;	/* Temp to keep bit 22 */
    long bit17;	/* Temp to keep bit 17 */
    long reg= 0x7ffff8; /* Initial value of internal register*/
};
} // namespace daisysp
#endif
#endif
