// This code is based on the code credited below, but it has been modified further
//
//  Biquad.h
//
//  Created by Nigel Redmon on 11/24/12
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the Biquad code:
//  http://www.earlevel.com/main/2012/11/25/biquad-c-source-code/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code
//  for your own purposes, free or commercial.
//

#pragma once
#ifndef DSY_BELABIQUAD_H
#define DSY_BELABIQUAD_H

#include <stdint.h>
#ifdef __cplusplus

namespace daisysp
{
/**
 * A class to compute Biquad filter coefficents and process biquad filters.
 */
class BelaBiquad {
	public:
		typedef enum
		{
			lowpass,
			highpass,
			bandpass,
			notch,
			peak,
			lowshelf,
			highshelf
		} Type;
		struct Settings {
			double fs; ///< Sample rate in Hz
			Type type; ///< Filter type
			double cutoff; ///< Cutoff in Hz
			double q; ///< Quality factor
			double peakGainDb; ///< Maximum filter gain
		};
		BelaBiquad() {}
		inline BelaBiquad(const BelaBiquad::Settings& settings) {
			Setup(settings);
		}
		~BelaBiquad() {}
		int Setup(const Settings& settings);


		/**
		 * Init to default settings
		 */ 

		inline void Init(float sample_rate) {

			BelaBiquad::Settings default_settings{
				.fs = sample_rate,
				.type = lowpass,
				.cutoff = 2000.0,
				.q = 2.0,  //4.0, //6.0, // 0.707,
				.peakGainDb = 0.0,
			};

			Setup(default_settings);
		}


		/**
		 * Process one input sample and return one output sample.
		 */
		float Process(float in);

		/**
		 * Reset the internal state of the filter to 0.
		 */
		void Clean();

		void SetType(Type type);
		void SetQ(double Q);
		void SetFreq(double Fc);
		void SetPeakGain(double peakGainDB);

		Type GetType();
		double GetQ();
		double GetFreq();
		double GetPeakGain();

		double GetStartingQ();
		double GetStartingFreq();
		double GetStartingPeakGain();

	protected:
		void CalcBiquad(void);

		Type type;
		double a0, a1, a2, b1, b2;
		double Fc, Q, peakGain;
		double Fs;
		double startFc, startQ, startPeakGain;
		double z1, z2;
};

inline float BelaBiquad::Process(float in) {
	double out = in * a0 + z1;
	z1 = in * a1 + z2 - b1 * out;
	z2 = in * a2 - b2 * out;
	return out;
}

} // namespace daisysp
#endif
#endif
