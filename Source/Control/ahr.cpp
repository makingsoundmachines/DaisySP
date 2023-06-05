#include <math.h>
#include "ahr.h"

using namespace daisysp;

float Ahr::Sigmoid(float x, float y) 
{ 
    return (x - x * y) / (y - abs(x) * 2 * y + 1); 
}

// Private Functions
void Ahr::Init(float sample_rate)
{
    samplerate = sample_rate;
	t = (1 / samplerate);
	time = 0.0f;
    env = 0.0f;

    stage		   = AHR_IDLE;
    //curve_scalar_  = 0.0f; // full linear

	stage_time[AHR_IDLE] = 0.05f;
	stage_time[AHR_ATTACK] = 0.05f;
	stage_time[AHR_DECAY] = 0.5f;

	stage_amp[AHR_IDLE] = 0.0f; // start - amp after attack
    stage_amp[AHR_ATTACK] = 1.0f; // end - amp after attack
    stage_amp[AHR_DECAY] = 0.0f; // 
}



// trig - trigger
// atk - attack time: stage_time[AHR_ATTACK]
// dec - decay time: stage_time[AHR_DECAY]
// start - start value
// end - end value
// env - output
// stage - enevlope segment
// time



float Ahr::Process()
{
	
	if (trig) {
		env = stage_amp[AHR_IDLE];
		time = 0.0f;
		stage = AHR_ATTACK;
	}

	// stage 0 = idle, 1 = attack, 2 = decay


    switch(stage)
    {
        case AHR_ATTACK: { // 1
			if (stage_time[AHR_ATTACK] <= 0) {
				env = 1.0f;
				time = 0.0f;
				stage = AHR_DECAY;
			}
			env = stage_amp[AHR_IDLE] + Sigmoid(time / stage_time[AHR_ATTACK], -0.25f) * (1.0f - stage_amp[AHR_IDLE]);
			time += t;

			if (time >= stage_time[AHR_ATTACK]) {
				env = 1.0f;
				time = 0.0f;
				stage = AHR_DECAY;
			}
		} break;
        case AHR_DECAY: { // 2
			if (stage_time[AHR_DECAY] <= 0) {
				env = 0.0f;
				time = 0.0f;
				stage = AHR_IDLE;
			}
			
			float div = time / stage_time[AHR_DECAY];
			curve = 0.75f;
			float off = 1.0f - curve;
			
			env = 1.0f - Sigmoid(div, -curve - div * off) * (1.0f - stage_amp[AHR_ATTACK]);
			time += t;

			if (time >= stage_time[AHR_DECAY]) {
				time = 0.0f;
				stage = AHR_IDLE;
			}
		} break;
        case AHR_IDLE: // 0
        default:
			env = stage_amp[AHR_ATTACK];
        break;
    }

	return env;
}