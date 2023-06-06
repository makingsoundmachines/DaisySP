#pragma once
#ifndef AHD_H
#define AHD_H

#include <stdint.h>
#ifdef __cplusplus

namespace daisysp
{
/** Distinct stages that the phase of the envelope can be located in.
- IDLE   = located at phase location 0, and not currently running
- ATTACK  = First segment of envelope where phase moves from 0 to 1
- HOLD    = Second segment of envelope the envelope is held at 1
- DECAY   = Third segment of envelope where phase moves from 1 to 0
*/
enum
{
    AHD_SEG_IDLE,
    AHD_SEG_ATTACK,
    AHD_SEG_HOLD,
    AHD_SEG_DECAY
};


/** Decay envelope module

Based on code by Steffan Diedrichsen, Ben Sergentanis, Paul Batchelor

Ahd envelope: Making Sound Machines

*/
class Ahd
{
  public:
    Ahd() {}
    ~Ahd() {}
    /** Initializes the Ahd module.
        \param sample_rate - The sample rate of the audio engine being run. 
    */
    void Init(float sample_rate, int blockSize = 1);
    /**
     \function Retrigger forces the envelope back to attack phase
     \param hard  resets the history to zero, results in a click.
     */
    void Trigger(bool hard);
    /** Processes one sample through the filter and returns one sample.
        \param gate - trigger the envelope, hold it to sustain 
    */
    float Process();
    /** Sets time
        Set time per segment in seconds
    */
    void SetTime(int seg, float time);
    void SetAttackTime(float timeInS, float shape = 0.0f);
    void SetHoldTime(float timeInS);
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
    inline bool IsRunning() const { return mode_ != AHD_SEG_IDLE; }
    /** Normalized tunable sigmoid by Dale H. Emery
        \return https://dhemery.github.io/DHE-Modules/technical/sigmoid/
    */
    float Sigmoid(float x, float y);    

  private:
    float   x_{0.f};
    uint32_t t_{0}; // ellapsed time in samples
    float   attackShape_{-1.f};
    float   attackTarget_{0.0f};
    float   attackTime_{-1.0f};
    float   holdTime_{-1.0f};
    uint32_t holdTime_samples_{0};
    float   decayTime_{-1.0f};
    float   attackD0_{0.f};
    float   decayD0_{0.f};
    int     sample_rate_;
    uint8_t mode_{AHD_SEG_IDLE};
    bool    gate_{false};
};
} // namespace daisysp
#endif
#endif
