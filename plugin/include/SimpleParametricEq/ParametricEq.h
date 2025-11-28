#pragma once
#include <juce_dsp/juce_dsp.h>
#include "SimpleParametricEq/filters/PeakFilter.h"
#include "SimpleParametricEq/filters/LowShelfFilter.h"

namespace parametric_eq {
class ParametricEq {
public:
    // Fixed for now
    static size_t const NUM_PEAKS = 4;
    static std::array<double, NUM_PEAKS> constexpr DEFAULT_FREQS = {100.0, 250.0, 1050.0, 2500.0};

    ParametricEq() = default;
    ~ParametricEq() = default;

    void prepare(double sampleRate, int numChannels);
    void reset();
    void processBlock(juce::AudioBuffer<float>& buffer);

    void prepareBandFilters();
    void setBandParameters(size_t bandIndex, double frequency, double Q, float gainDb);
private:
    std::array<PeakFilter, NUM_PEAKS> peakFilters_;
    double sampleRate_{44100.0};
    int numChannels_;
};
} // namespace parametric_eq