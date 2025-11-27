#include "SimpleParametricEq/components/PeakFilter.h"

void PeakFilter::prepare(double sampleRate, double frequency, double Q) {
    sampleRate_ = sampleRate;
    frequency_ = frequency;
    QRaw_ = Q;
    gainDbRaw_ = 0.0f;

    qSmoothed_.reset(sampleRate_, 0.02); 
    gainSmoothed_.reset(sampleRate_, 0.01); 

    qSmoothed_.setCurrentAndTargetValue(static_cast<float>(QRaw_));

    const float initA = std::pow(10.0f, gainDbRaw_ / 20.0f);
    gainSmoothed_.setCurrentAndTargetValue(initA);

    calculateCoefficients(static_cast<double>(qSmoothed_.getCurrentValue()), gainSmoothed_.getCurrentValue());

    reset();
}

void PeakFilter::reset() {
    std::fill(z1_.begin(), z1_.end(), 0.0f);
    std::fill(z2_.begin(), z2_.end(), 0.0f);
}

void PeakFilter::calculateCoefficients(double Q, float amplitude) {
    const auto w0 = 2.0 * M_PI * frequency_ / sampleRate_;
    const auto cos_w = static_cast<float>(std::cos(w0));
    const auto alpha = static_cast<float>(std::sin(w0) / (2.0 * Q));

    const auto A = amplitude; 

    a0 = 1.0f + (alpha / A);

    b0 = 1.0f + (alpha * A);
    b1 = -2.0f * cos_w;
    b2 = 1.0f - (alpha * A);
    a1 = -2.0f * cos_w;
    a2 = 1.0f - (alpha / A);

    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
}


void PeakFilter::processBlock(juce::AudioBuffer<float>& buffer) {
    const auto numChannels = static_cast<size_t>(buffer.getNumChannels());
    const auto numSamples = buffer.getNumSamples();

    if (z1_.size() != numChannels) {
        z1_.assign(numChannels, 0.0f);
        z2_.assign(numChannels, 0.0f);
    }

    auto lastQ = qSmoothed_.getCurrentValue();
    auto lastA = gainSmoothed_.getCurrentValue();

    if (coeffsDirty_) {
        calculateCoefficients(static_cast<double>(lastQ), lastA);
        coeffsDirty_ = false;
    }

    for (int n = 0; n < numSamples; ++n) {
        const auto qNow = qSmoothed_.getNextValue();
        const auto aNow = gainSmoothed_.getNextValue();

        const auto aDiff = std::abs(aNow - lastA);
        const auto qDiff = std::abs(qNow - lastQ);

        if (qDiff > EPSILON || aDiff > EPSILON) {
            calculateCoefficients(static_cast<double>(qNow), aNow);
            lastQ = qNow;
            lastA = aNow;
        }

        for (size_t ch = 0; ch < numChannels; ++ch) {
            auto* channelData = buffer.getWritePointer(static_cast<int>(ch));
            auto x = channelData[n];

            auto z1 = z1_[ch];
            auto z2 = z2_[ch];

            const auto y = b0 * x + z1;
            z1 = b1 * x - a1 * y + z2;
            z2 = b2 * x - a2 * y;

            channelData[n] = y;

            z1_[ch] = z1;
            z2_[ch] = z2;
        }
    }
}

void PeakFilter::setFrequency(double frequency) {
    frequency_ = frequency;
    coeffsDirty_ = true;
}

void PeakFilter::setQ(double Q) {
    QRaw_ = Q;
    qSmoothed_.setTargetValue(static_cast<float>(QRaw_));
    coeffsDirty_ = true;
}

void PeakFilter::setAmplitude(float gainDb) {
    gainDbRaw_ = gainDb;
    const float A = std::pow(10.0f, gainDbRaw_ / 20.0f);
    gainSmoothed_.setTargetValue(A); 
    coeffsDirty_ = true;
}