#pragma once
#ifndef ADE_H
#define ADE_H

#include <stdint.h>
#ifdef __cplusplus

namespace daisysp
{
/** Distinct stages that the phase of the envelope can be located in.
- IDLE   = located at phase location 0, and not currently running
- ATTACK  = First segment of envelope where phase moves from 0 to 1
- DECAY   = Second segment of envelope where phase moves from 1 to SUSTAIN value
- RELEASE =     Fourth segment of envelop where phase moves from SUSTAIN to 0
*/
enum
{
    ADE_SEG_IDLE,
    ADE_SEG_ATTACK,
    ADE_SEG_DECAY,
    ADE_SEG_END
};


/** Ade envelope module

Original author(s) : Paul Batchelor

Ported from Soundpipe by Ben Sergentanis, May 2020
 
Remake by Steffan DIedrichsen, May 2021
*/
class Ade
{
  public:
    Ade() {}
    ~Ade() {}
    /** Initializes the Ade module.
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
    void SetDecayTime(float timeInS);
    void SetReleaseTime(float timeInS);

  private:
    void SetTimeConstant(float timeInS, float& time, float& coeff);
    uint8_t mode_{ADE_SEG_IDLE};    

  public:
    /** Sustain level
        \param sus_level - sets sustain level, 0...1.0
    */
    inline void SetSustainLevel(float sus_level)
    {
        if(sus_level <= 0.f) { 
            // force envelope into idle
            sus_level = -0.01f; 
            mode_ = ADE_SEG_IDLE;
        } else if(sus_level > 1.f) {
            sus_level = 1.f;
        }
        sus_level_ = sus_level;
    }
    /** get the current envelope segment
        \return the segment of the envelope that the phase is currently located in.
    */
    inline uint8_t GetCurrentSegment() { return mode_; }
    /** Tells whether envelope is active
        \return true if the envelope is currently in any stage apart from idle.
    */
    inline bool IsRunning() const { return mode_ != ADE_SEG_IDLE; }

  private:
    float   sus_level_{0.f};
    float   x_{0.f};
    float   attackShape_{-1.f};
    float   attackTarget_{0.0f};
    float   attackTime_{-1.0f};
    float   decayTime_{-1.0f};
    float   releaseTime_{-1.0f};
    float   attackD0_{0.f};
    float   decayD0_{0.f};
    int     sample_rate_;
    bool    gate_{false};
};
} // namespace daisysp
#endif
#endif
