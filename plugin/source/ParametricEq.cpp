#include "SimpleParametricEq/ParametricEq.h"

namespace parametric_eq {
void ParametricEq::prepare(double sampleRate) {
    sampleRate_ = sampleRate;
    prepareBandFilters();
}

void ParametricEq::reset() {
    for (auto &filter : notchFilters_) {
        filter.reset();
    }
}

void ParametricEq::processBlock(juce::AudioBuffer<float>& buffer) {
    for (auto &filter : notchFilters_) {
        filter.processBlock(buffer);
    }
}

void ParametricEq::prepareBandFilters() {
    auto interval = {100.0, 250.0, 1050.0, 2500.0};
    auto totalBandFilters = static_cast<int>(notchFilters_.size());
    for (int i = 0; i < totalBandFilters; ++i) {
        jassert(static_cast<size_t>(i) < NUM_BANDS);
        auto freq = *std::next(interval.begin(), i);
        notchFilters_[static_cast<size_t>(i)].prepare(sampleRate_, freq, 1.0);
    }
}

void ParametricEq::setBandParameters(size_t bandIndex, double frequency, double Q, float gainDb) {
    if (bandIndex >= notchFilters_.size() || bandIndex < 0) {
        return;
    } else if (bandIndex < notchFilters_.size() && bandIndex >= 0) {
        notchFilters_[bandIndex].setFrequency(frequency);
        notchFilters_[bandIndex].setQ(Q);
        notchFilters_[bandIndex].setAmplitude(gainDb);
    }
}
} // namespace parametric_eq