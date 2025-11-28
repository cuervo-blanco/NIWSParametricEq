#pragma once

#include "BiquadFilter.h"

class NotchFilter : public BiquadFilter {
public:
    NotchFilter() = default;
    ~NotchFilter() override = default;

    void setParametersAndReset(double frequency, double Q) {
        frequency_ = frequency;

        QRaw_ = Q;
        qSmoothed_.reset(sampleRate_, 0.02); 
        qSmoothed_.setCurrentAndTargetValue(static_cast<float>(QRaw_));
        lastQ_ = qSmoothed_.getCurrentValue();

        calculateAndSetCoefficients(lastQ_);
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

protected:
    void updateSmoothedParameters() override {
        if (coeffsDirty_) {
            calculateAndSetCoefficients(qSmoothed_.getCurrentValue());
            lastQ_ = qSmoothed_.getCurrentValue();
            coeffsDirty_ = false;
            return;
        }

        const auto qNow = qSmoothed_.getNextValue();

        const auto qDiff = std::abs(qNow - lastQ_);

        if (qDiff > EPSILON) {
            calculateAndSetCoefficients(qNow);
            lastQ_ = qNow;
        }
    }

private:
    void calculateAndSetCoefficients(float Q) {
        const auto w0 = static_cast<float>(2.0 * M_PI * frequency_ / sampleRate_);
        const auto cos_w = static_cast<float>(std::cos(w0));
        const auto alpha = std::sin(w0) / (2.0f * Q);

        float a0 = 1.0f + alpha;

        float b0 = 1.0f;
        float b1 = -2.0f * cos_w;
        float b2 = 1.0f;
        float a1 = -2.0f * cos_w;
        float a2 = 1.0f - alpha;

        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
        a1 /= a0;
        a2 /= a0;

        setCoefficients(b0, b1, b2, a0, a1, a2);
    }

    double frequency_{1000.0};
    double QRaw_{1.0};

    juce::SmoothedValue<float> qSmoothed_;

    float lastQ_{1.0f};

    bool coeffsDirty_{true};
};