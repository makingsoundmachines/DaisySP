#pragma once
#ifndef DSY_DEC_H
#define DSY_DEC_H

#include <stdint.h>
#ifdef __cplusplus

namespace daisysp
{
/** Distinct stages that the phase of the envelope can be located in.
- IDLE   = located at phase location 0, and not currently running
- DECAY = Decay segment of the envelope where phase moves from 1 to 0
*/
enum
{
    DEC_SEG_IDLE,
    DEC_SEG_DECAY
};


/** Decay envelope module

Based on code by Steffan Diedrichsen, Ben Sergentanis, Paul Batchelor

Decay-only envelope: Making Sound Machines

*/
class Dec
{
  public:
    Dec() {}
    ~Dec() {}
    /** Initializes the Dec module.
        \param sample_rate - The sample rate of the audio engine being run. 
    */
    void Init(float sample_rate, int blockSize = 1);
    /**
     \function Retrigger forces the envelope back to attack phase
     \param hard  resets the history to zero, results in a click.
     */
    void Trigger();
    /** Processes one sample through the filter and returns one sample.
        \param gate - trigger the envelope, hold it to sustain 
    */
    float Process();
    /** Sets time
        Set time per segment in seconds
    */
    void SetTime(int seg, float time);
    void SetDecayTime(float timeInS);

  private:
    void SetTimeConstant(float timeInS, float& time, float& coeff);

  public:
    /** get the current envelope segment
        \return the segment of the envelope that the phase is currently located in.
    */
    inline uint8_t GetCurrentSegment() { return mode_; }
    /** Tells whether envelope is active
        \return true if the envelope is currently in any stage apart from idle.
    */
    inline bool IsRunning() const { return mode_ != DEC_SEG_IDLE; }

  private:
    float   sus_level_{0.f};
    float   x_{0.f};
    float   decayTime_{-1.0f};
    float   decayD0_{0.f};
    int     sample_rate_;
    uint8_t mode_{DEC_SEG_IDLE};
};
} // namespace daisysp
#endif
#endif
