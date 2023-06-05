#pragma once
#ifndef AHR_H
#define AHR_H

#include <stdint.h>
#ifdef __cplusplus

namespace daisysp
{
/** Distinct stages that the phase of the envelope can be located in.
- IDLE    = located at phase location 0, and not currently running
- ATTACK  = First segment of envelope where phase moves from 0 to 1
- DECAY   = Second segment of envelope where phase moves from 1 to SUSTAIN value
- LAST    = The final segment of the envelope (currently decay)
*/
enum AhrEnvSegment
{
    AHR_IDLE,
    AHR_ATTACK,
    AHR_DECAY,
    AHR_LAST,
};

/** Trigger-able AHR envelope
*/
class Ahr
{
  public:
    Ahr() {}
    ~Ahr() {}
    /** Initializes the ad envelope.
        Defaults:
            - current segment = idle
            - curve = linear
            - phase = 0
            - min = 0
            - max = 1
        \param sample_rate      sample rate of the audio engine being run
    */
    void Init(float sample_rate);

    /** Processes the current sample of the envelope. This should be called once
        per sample period.
        \return the current envelope value.
    */
    float Process();

    /** Starts or retriggers the envelope.*/
    inline void Trigger() { trig = 1; }
    /** Sets the length of time (in seconds) for a specific stage. */
    inline void SetTime(uint8_t seg, float time) { stage_time[seg] = time; }
    /** Sets the amount of curve applied. A positve value will create a log
        curve. Input range: -100 to 100.  (or more)
    */
    // inline void SetCurve(float scalar) { curve_scalar_ = scalar; }
    /** Returns the current output value without processing the next sample */
    inline float GetValue() const { return env; }
    /** Returns the segment of the envelope that the phase is currently located
        in.
    */
    inline uint8_t GetCurrentSegment() { return stage; }
    /** Returns true if the envelope is currently in any stage apart from idle.
    */
    inline bool IsRunning() const { return stage != AHR_IDLE; }
    /** Normalized tunable sigmoid by Dale H. Emery
        \return https://dhemery.github.io/DHE-Modules/technical/sigmoid/
    */
    float Sigmoid(float x, float y);

  private:
    bool     trig{false};
    float    stage_time[AHR_LAST];
    float    stage_amp[AHR_LAST];
    uint8_t  stage{AHR_IDLE};
    float    samplerate{48000.f};
    float    t{1/48000.f};
    float    env{0.0f}; // output
    float    curve{0.75f};
    float    time{0.0f};     
};

} // namespace daisysp
#endif
#endif