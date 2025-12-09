#pragma once
#include <juce_dsp/juce_dsp.h>
#include "SimpleParametricEq/filters/PeakFilter.h"
#include "SimpleParametricEq/filters/LowShelfFilter.h"
#include "SimpleParametricEq/filters/LowPassFilter.h"
#include "SimpleParametricEq/filters/HighPassFilter.h"
#include "filters/BiquadFilter.h"

namespace parametric_eq {
enum class Slope : uint8_t {
    dB12 = 0,
    dB24 = 1,
    dB36 = 2,
    dB48 = 3, 
    dB96 = 4
};
class ParametricEq {
public:
    static size_t const NUM_PEAKS = 4;
    static std::array<double, NUM_PEAKS> constexpr DEFAULT_FREQS = {100.0, 250.0, 1050.0, 2500.0};

    ParametricEq() = default;
    ~ParametricEq() = default;

    void prepare(double sampleRate, int numChannels);
    void reset();
    void processBlock(juce::AudioBuffer<float>& buffer);

    void prepareFilters();

    void setPeakParameters(size_t bandIndex, double frequency, double Q, float gainDb, bool isBypassed);
    void setLowShelfParameters(double frequency, double Q, float gainDb, bool isBypassed);
    void setLowPassParameters(double frequency, double Q, bool isBypassed, int slopeIndex);
    void setHighPassParameters(double frequency, double Q, bool isBypassed, int slopeIndex);

    std::vector<BiquadFilter*> getBands() noexcept;
    BiquadFilter& getPeakFilter(size_t index);
private:
    static constexpr int MAX_SLOPE_SECTIONS = 8; 

    int numLowPassSections_ = 1;
    int numHighPassSections_ = 1;

    std::array<PeakFilter, NUM_PEAKS> peakFilters_;
    LowShelfFilter lowShelfFilter_;
    std::array<LowPassFilter, MAX_SLOPE_SECTIONS> lowPassFilters_;
    std::array<HighPassFilter, MAX_SLOPE_SECTIONS> highPassFilters_;

    double sampleRate_{44100.0};
    int numChannels_;
};
} // namespace parametric_eq