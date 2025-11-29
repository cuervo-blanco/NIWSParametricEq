#include "SimpleParametricEq/ParametricEq.h"

namespace parametric_eq {
void ParametricEq::prepare(double sampleRate, int numChannels) {
    sampleRate_ = sampleRate;
    numChannels_ = numChannels;
    prepareFilters();
}

void ParametricEq::reset() {
    for (auto &filter : peakFilters_) {
        filter.reset();
    }
    lowShelfFilter_.reset();
}

void ParametricEq::processBlock(juce::AudioBuffer<float>& buffer) {
    for (auto &filter : peakFilters_) {
        filter.processBlock(buffer);
    }
    lowShelfFilter_.processBlock(buffer);
    lowPassFilter_.processBlock(buffer);
    highPassFilter_.processBlock(buffer);
}

void ParametricEq::prepareFilters() {
    auto totalBandFilters = static_cast<int>(peakFilters_.size());
    for (int i = 0; i < totalBandFilters; ++i) {
        jassert(static_cast<size_t>(i) < NUM_PEAKS);
        auto freq = *std::next(DEFAULT_FREQS.begin(), i);
        peakFilters_[static_cast<size_t>(i)].prepare(sampleRate_, numChannels_);
        peakFilters_[static_cast<size_t>(i)].setParametersAndReset(freq, 1.0);
    }

    lowShelfFilter_.prepare(sampleRate_, numChannels_);
    lowShelfFilter_.setParametersAndReset(80.0, 1.0);

    lowPassFilter_.prepare(sampleRate_, numChannels_);
    lowPassFilter_.setParametersAndReset(150000.0, 1.0);

    highPassFilter_.prepare(sampleRate_, numChannels_);
    highPassFilter_.setParametersAndReset(40.0, 1.0);
}

void ParametricEq::setPeakParameters(size_t bandIndex, 
    double frequency, double Q, float gainDb, bool isBypassed) {
    if (bandIndex >= peakFilters_.size() || bandIndex < 0) {
        return;
    } else if (bandIndex < peakFilters_.size() && bandIndex >= 0) {
        peakFilters_[bandIndex].setFrequency(frequency);
        peakFilters_[bandIndex].setQ(Q);
        peakFilters_[bandIndex].setAmplitude(gainDb);
        peakFilters_[bandIndex].setBypassed(isBypassed);
    }
}

void ParametricEq::setLowShelfParameters(double frequency, double Q, 
    float gainDb, bool isBypassed) {
    lowShelfFilter_.setFrequency(frequency);
    lowShelfFilter_.setQ(Q);
    lowShelfFilter_.setAmplitude(gainDb);
    lowShelfFilter_.setBypassed(isBypassed);
}

void ParametricEq::setLowPassParameters(double frequency, double Q, bool isBypassed) {
    lowPassFilter_.setFrequency(frequency);
    lowPassFilter_.setQ(Q);
    lowPassFilter_.setBypassed(isBypassed);
}

void ParametricEq::setHighPassParameters(double frequency, double Q, bool isBypassed) {
    highPassFilter_.setFrequency(frequency);
    highPassFilter_.setQ(Q);
    highPassFilter_.setBypassed(isBypassed);
}

} // namespace parametric_eq