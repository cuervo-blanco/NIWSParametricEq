#pragma once

#include "BiquadFilter.h"

class LowShelfFilter : public BiquadFilter {
private:
    void calculateAndSetCoefficients(float Q, float amplitude) {
        const auto w0 = static_cast<float>(2.0 * M_PI * frequency_ / sampleRate_);
        const auto cos_w = static_cast<float>(std::cos(w0));
        const auto alpha = std::sin(w0) / (2.0f * Q);

        const auto A = amplitude;
        const auto twoSqrtAlpha = 2.f * std::sqrt(A) * alpha;

        float a0 = (A + 1.f) + (A - 1.f) * cos_w + twoSqrtAlpha;

        float b0 = A * ((A + 1.f) - (A - 1.f) * cos_w + twoSqrtAlpha);
        float b1 = 2.f * A * ((A - 1.f) - (A + 1.f) * cos_w);
        float b2 = A * ((A + 1.f) - (A - 1.f) * cos_w) - twoSqrtAlpha;
        float a1 = -2.f * ((A - 1.f) + (A + 1.f) * cos_w);
        float a2 = (A + 1.f) + (A - 1.f) * cos_w - twoSqrtAlpha;

        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
        a1 /= a0;
        a2 /= a0;

        setCoefficients(b0, b1, b2, a0, a1, a2);
    }

    double frequency_{1000.0};
    double QRaw_{1.0};
    float gainDbRaw_{0.0f};

    juce::SmoothedValue<float> qSmoothed_;
    juce::SmoothedValue<float> gainSmoothed_;

    float lastQ_{1.0f};
    float lastA_{1.0f};

    bool coeffsDirty_{true};

public:
    LowShelfFilter() = default;
    ~LowShelfFilter() override = default;

    void setParametersAndReset(double frequency, double Q, float amplitude = 0.0f) {
        frequency_ = frequency;
        QRaw_ = Q;
        gainDbRaw_ = amplitude;

        qSmoothed_.reset(sampleRate_, 0.02); 
        gainSmoothed_.reset(sampleRate_, 0.01);

        qSmoothed_.setCurrentAndTargetValue(static_cast<float>(QRaw_));

        const float initA = std::pow(10.0f, gainDbRaw_ / 20.0f);
        gainSmoothed_.setCurrentAndTargetValue(initA);

        lastQ_ = qSmoothed_.getCurrentValue();
        lastA_ = gainSmoothed_.getCurrentValue();

        calculateAndSetCoefficients(lastQ_, lastA_);

        reset();
    }

    void setFrequency(double frequency) {
        frequency_ = frequency;
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

protected:
    void updateSmoothedParameters() override {
        if (coeffsDirty_) {
            calculateAndSetCoefficients(qSmoothed_.getCurrentValue(),
                                        gainSmoothed_.getCurrentValue());
            lastQ_ = qSmoothed_.getCurrentValue();
            lastA_ = gainSmoothed_.getCurrentValue();
            coeffsDirty_ = false;
            return;
        }

        const auto qNow = qSmoothed_.getNextValue();
        const auto aNow = gainSmoothed_.getNextValue();

        const auto qDiff = std::abs(qNow - lastQ_);
        const auto aDiff = std::abs(aNow - lastA_);

        if (qDiff > EPSILON || aDiff > EPSILON) {
            calculateAndSetCoefficients(qNow, aNow);
            lastQ_ = qNow;
            lastA_ = aNow;
        }
    }
};