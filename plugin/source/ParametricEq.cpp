#include "SimpleParametricEq/ParametricEq.h"

namespace parametric_eq {
void ParametricEq::prepare(double sampleRate, int numChannels) {
    sampleRate_ = sampleRate;
    numChannels_ = numChannels;
    preparePeakFilters();
}

void ParametricEq::reset() {
    for (auto &filter : peakFilters_) {
        filter.reset();
    }
}

void ParametricEq::processBlock(juce::AudioBuffer<float>& buffer) {
    for (auto &filter : peakFilters_) {
        filter.processBlock(buffer);
    }
}

void ParametricEq::preparePeakFilters() {
    auto totalBandFilters = static_cast<int>(peakFilters_.size());
    for (int i = 0; i < totalBandFilters; ++i) {
        jassert(static_cast<size_t>(i) < NUM_PEAKS);
        auto freq = *std::next(DEFAULT_FREQS.begin(), i);
        peakFilters_[static_cast<size_t>(i)].prepare(sampleRate_, numChannels_);
        peakFilters_[static_cast<size_t>(i)].setParametersAndReset(freq, 1.0);
    }
}

void ParametricEq::setPeakParameters(size_t bandIndex, double frequency, double Q, float gainDb) {
    if (bandIndex >= peakFilters_.size() || bandIndex < 0) {
        return;
    } else if (bandIndex < peakFilters_.size() && bandIndex >= 0) {
        peakFilters_[bandIndex].setFrequency(frequency);
        peakFilters_[bandIndex].setQ(Q);
        peakFilters_[bandIndex].setAmplitude(gainDb);
    }
}
} // namespace parametric_eq