#pragma once
#ifndef FM_UTILS_H
#define FM_UTILS_H

#ifdef __cplusplus

namespace daisysp
{


/** FM utilities module

*/
class FM_utility
{
  public:
    FM_utility() {}
    ~FM_utility() {}

    /** Normalized tunable sigmoid by Dale H. Emery
        \return https://dhemery.github.io/DHE-Modules/technical/sigmoid/
    */
    float Sigmoid(float x, float k);

    /** tanh() approx from JUCE
    */
    float tanh_approx(float x);

    /** tan() approx by michael massberg
    */
    float tan_approx(float x);

    /** curve
    */
    float curve(float x);

    /** smooth
    */
    float smooth(float x, float time);

  // private:
};
} // namespace daisysp
#endif
#endif
