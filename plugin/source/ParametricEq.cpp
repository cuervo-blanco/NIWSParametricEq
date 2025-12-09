#include "SimpleParametricEq/ParametricEq.h"

namespace parametric_eq {
static int slopeToSections(Slope s) {
    switch (s) {
        case Slope::dB12: return 1;
        case Slope::dB24: return 2;
        case Slope::dB36: return 3;
        case Slope::dB48: return 4;
        case Slope::dB96: return 8;
    }
    return 1;
}
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

    for (int i = 0; i < numLowPassSections_; ++i) {
        lowPassFilters_[static_cast<size_t>(i)].processBlock(buffer);
    }

    for (int i = 0; i < numHighPassSections_; ++i) {
        highPassFilters_[static_cast<size_t>(i)].processBlock(buffer);
    }
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

    for (int i = 0; i < MAX_SLOPE_SECTIONS; ++i) {
        highPassFilters_[static_cast<size_t>(i)].prepare(sampleRate_, numChannels_);
        highPassFilters_[static_cast<size_t>(i)].setParametersAndReset(40.0, 1.0);
    }

    for (int i = 0; i < MAX_SLOPE_SECTIONS; ++i) {
        lowPassFilters_[static_cast<size_t>(i)].prepare(sampleRate_, numChannels_);
        lowPassFilters_[static_cast<size_t>(i)].setParametersAndReset(15000.0, 1.0);
    }
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

void ParametricEq::setLowPassParameters(double frequency, double Q, bool isBypassed, int slopeIndex) {
    Slope slope = static_cast<Slope>(slopeIndex);
    numLowPassSections_ = juce::jlimit(1, MAX_SLOPE_SECTIONS, slopeToSections(slope));
    for (int i = 0; i < numLowPassSections_; ++i) {
        lowPassFilters_[static_cast<size_t>(i)].setFrequency(frequency);
        lowPassFilters_[static_cast<size_t>(i)].setQ(Q);
        lowPassFilters_[static_cast<size_t>(i)].setBypassed(isBypassed);
    }
}

void ParametricEq::setHighPassParameters(double frequency, double Q, bool isBypassed, int slopeIndex) {
    Slope slope = static_cast<Slope>(slopeIndex);
    numHighPassSections_ = juce::jlimit(1, MAX_SLOPE_SECTIONS, slopeToSections(slope));
    for (int i = 0; i < numHighPassSections_; ++i) {
        highPassFilters_[static_cast<size_t>(i)].setFrequency(frequency);
        highPassFilters_[static_cast<size_t>(i)].setQ(Q);
        highPassFilters_[static_cast<size_t>(i)].setBypassed(isBypassed);
    }
}

std::vector<BiquadFilter*> ParametricEq::getBands() noexcept {
    std::vector<BiquadFilter*> result;

    for (auto& p : peakFilters_) {
        result.push_back(&p);
    }

    result.push_back(&lowShelfFilter_);

    for (int i = 0; i < numLowPassSections_; ++i) {
        result.push_back(&lowPassFilters_[static_cast<size_t>(i)]);
    }

    for (int i = 0; i < numHighPassSections_; ++i) {
        result.push_back(&highPassFilters_[static_cast<size_t>(i)]);
    }

    return result;
}


BiquadFilter& ParametricEq::getPeakFilter(size_t index) {
    jassert(index < NUM_PEAKS);
    return peakFilters_[index];
}

} // namespace parametric_eq
