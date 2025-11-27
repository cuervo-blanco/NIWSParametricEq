#pragma once
#include <juce_dsp/juce_dsp.h>
#include "SimpleParametricEq/components/NotchFilter.h"

namespace parametric_eq {
class ParametricEq {
public:
    // Fixed for now
    static size_t const NUM_BANDS = 4;

    ParametricEq() = default;
    ~ParametricEq() = default;

    void prepare(double sampleRate);
    void reset();
    void processBlock(juce::AudioBuffer<float>& buffer);

    void prepareBandFilters();
    void setBandParameters(size_t bandIndex, double frequency, double Q, float gainDb);
private:
    std::array<NotchFilter, NUM_BANDS> notchFilters_;
    double sampleRate_{44100.0};
};
} // namespace parametric_eq