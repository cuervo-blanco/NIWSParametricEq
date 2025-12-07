#pragma once

#include <vector>
#include <cmath>
#include <juce_dsp/juce_dsp.h>

class BiquadFilter {
public:
    virtual ~BiquadFilter() = default;
    virtual void prepare(double sampleRate, int numChannels) {
        sampleRate_ = sampleRate;
        numChannels_ = numChannels;

        z1_.assign(static_cast<size_t>(numChannels_), 0.0f);
        z2_.assign(static_cast<size_t>(numChannels_), 0.0f);

        bypassMix_.reset(sampleRate_, 0.005);
        bypassMix_.setCurrentAndTargetValue(isBypassed_ ? 0.0f : 1.0f); 
    }

    virtual void reset() {
        std::fill(z1_.begin(), z1_.end(), 0.0f);
        std::fill(z2_.begin(), z2_.end(), 0.0f);
    }

    void processBlock(juce::AudioBuffer<float>& buffer) {
        const auto numChannels = buffer.getNumChannels();
        const auto numSamples = buffer.getNumSamples();

        if (numChannels != numChannels_) {
            prepare(sampleRate_, numChannels);
        }

        for (int n = 0; n < numSamples; ++n) {
            updateSmoothedParameters();
            float mix = bypassMix_.getNextValue();
            if (mix <= EPSILON) {
                continue;
            }

            for (int ch = 0; ch < numChannels; ++ch) {
                auto* channelData = buffer.getWritePointer(ch);
                auto x = channelData[n];

                auto& z1 = z1_[static_cast<size_t>(ch)];
                auto& z2 = z2_[static_cast<size_t>(ch)];

                const auto y = b0_ * x + z1;
                z1 = b1_ * x - a1_ * y + z2;
                z2 = b2_ * x - a2_ * y;

                channelData[n] = x + mix * (y - x);
            }
        }
    }

    void setParametersAndReset(double frequency, double Q, float amplitude = 0.0f) {
        freqRaw_ = frequency;
        QRaw_ = Q;
        gainDbRaw_ = amplitude;

        qSmoothed_.reset(sampleRate_, 0.02); 
        gainSmoothed_.reset(sampleRate_, 0.01);
        freqSmoothed_.reset(sampleRate_, 0.01);

        qSmoothed_.setCurrentAndTargetValue(static_cast<float>(QRaw_));
        freqSmoothed_.setCurrentAndTargetValue(static_cast<float>(freqRaw_));

        const auto initA = std::pow(10.0f, gainDbRaw_ / 20.0f);
        gainSmoothed_.setCurrentAndTargetValue(initA);

        lastQ_ = qSmoothed_.getCurrentValue();
        lastA_ = gainSmoothed_.getCurrentValue();
        lastFreq_ = freqSmoothed_.getCurrentValue();

        calculateAndSetCoefficients(lastQ_, lastA_, lastFreq_);

        reset();
    }

    void setBypassed(bool shouldBypass) noexcept {
        isBypassed_ = shouldBypass;
        bypassMix_.setTargetValue(shouldBypass ? 0.0f : 1.0f);
    }

    void setFrequency(double frequency) {
        freqRaw_ = frequency;
        freqSmoothed_.setTargetValue(static_cast<float>(freqRaw_));
        coeffsDirty_ = true;
    }

    void setQ(double Q) {
        QRaw_ = Q;
        qSmoothed_.setTargetValue(static_cast<float>(QRaw_));
    }

    void setAmplitude(float gainDb) {
        gainDbRaw_ = gainDb;
        const float A = std::pow(10.0f, gainDbRaw_ / 20.0f);
        gainSmoothed_.setTargetValue(A);
    }

    float getMagnitudeAtFrequency(double freq) const {
        if (sampleRate_ <= 0.0 || isBypassed_) {
            return 1.0f;
        }

        const double omega = juce::MathConstants<double>::twoPi * freq / sampleRate_;
        const double cos1  = std::cos(omega);
        const double cos2  = std::cos(2.0 * omega);

        const auto b0 = static_cast<double>(b0_);
        const auto b1 = static_cast<double>(b1_);
        const auto b2 = static_cast<double>(b2_);
        const auto a1 = static_cast<double>(a1_);
        const auto a2 = static_cast<double>(a2_);

        const auto numerator =
            b0*b0 + b1*b1 + b2*b2
            + 2.0 * (b0*b1 + b1*b2) * cos1
            + 2.0 *  b0*b2 * cos2;

        const double denominator =
            1.0 + a1*a1 + a2*a2
            + 2.0 * (a1 + a1*a2) * cos1
            + 2.0 * a2 * cos2;

        if (denominator <= 0.0 || numerator <= 0.0) {
            return 1.0f; 
        }

        const auto mag = std::sqrt(numerator / denominator);
        return static_cast<float>(mag);
    }

    float getMagnitudeDbAt(double frequencyHz) const noexcept {
        return juce::Decibels::gainToDecibels(getMagnitudeAtFrequency(frequencyHz));
    }

protected:
    static constexpr float EPSILON = 1e-3f;

    double freqRaw_{1000.0};
    double QRaw_{1.0};
    float gainDbRaw_{0.0f};

    void setCoefficients(float b0, float b1, float b2, float a0, float a1, float a2) {
        b0_ = b0;
        b1_ = b1;
        b2_ = b2;
        a0_ = a0;
        a1_ = a1;
        a2_ = a2;
    }

    double sampleRate_{44100.0};
    int numChannels_{0};

    float b0_{1.0f};
    float b1_{0.0f};
    float b2_{0.0f};
    float a0_{0.0f};
    float a1_{0.0f};
    float a2_{0.0f};

    std::vector<float> z1_;
    std::vector<float> z2_;

    juce::LinearSmoothedValue<float> bypassMix_{1.0f};
    bool isBypassed_{false};

    juce::SmoothedValue<float> qSmoothed_;
    juce::SmoothedValue<float> gainSmoothed_;
    juce::SmoothedValue<float> freqSmoothed_;

    float lastQ_{1.0f};
    float lastA_{1.0f};
    float lastFreq_{1000.0f};

    bool coeffsDirty_{true};

    virtual void calculateAndSetCoefficients(float Q, float amplitude, float frequency) = 0;

    void updateSmoothedParameters() {
        if (coeffsDirty_) {
            calculateAndSetCoefficients(qSmoothed_.getCurrentValue(),
                                        gainSmoothed_.getCurrentValue(),
                                        freqSmoothed_.getCurrentValue());
            lastQ_ = qSmoothed_.getCurrentValue();
            lastA_ = gainSmoothed_.getCurrentValue();
            lastFreq_ = freqSmoothed_.getCurrentValue();
            coeffsDirty_ = false;
            return;
        }

        const auto qNow = qSmoothed_.getNextValue();
        const auto aNow = gainSmoothed_.getNextValue();
        const auto freqNow = freqSmoothed_.getNextValue();

        const auto qDiff = std::abs(qNow - lastQ_);
        const auto aDiff = std::abs(aNow - lastA_);
        const auto freqDiff = std::abs(freqNow - lastFreq_);

        if (qDiff > EPSILON || aDiff > EPSILON || freqDiff > EPSILON) {
            calculateAndSetCoefficients(qNow, aNow, freqNow);
            lastQ_ = qNow;
            lastA_ = aNow;
            lastFreq_ = freqNow;
        }
    }
};
