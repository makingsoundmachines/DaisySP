#include "fm_utils.h"
#include <math.h>

using namespace daisysp;

float FM_utility::Sigmoid(float x, float y) 
{ 
    return (x - x * y) / (y - abs(x) * 2 * y + 1); 
}

float FM_utility::tanh_approx(float x)
{
    float x2 = x * x;
    float numerator = x * (135135 + x2 * (17325 + x2 * (378 + x2)));
    float denominator = 135135 + x2 * (62370 + x2 * (3150 + 28 * x2));
    return numerator / denominator;
}

float FM_utility::tan_approx(float x) 
{
	float x2 = x * x;
    float numerator = x * (0.999999492001 + x2 * -0.096524608111);
    float denominator = 1 + x2 * (-0.429867256894 + x2 * 0.009981877999);
	return numerator / denominator;
}

float FM_utility::curve(float x) {
	return x / (x + (1 - 1 / x) * (x - 1));
}

float FM_utility::smooth(float x, float time) { 
	float prev = 0.0f;
	prev = prev + time * (x - prev);		

	return prev;
}
