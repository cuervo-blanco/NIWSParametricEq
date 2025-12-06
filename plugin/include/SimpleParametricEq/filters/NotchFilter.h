#pragma once

#include "FilterParameters.h"

class NotchFilter : public BiquadFilter {
public:
    NotchFilter() = default;
    ~NotchFilter() override = default;
private:
    void calculateAndSetCoefficients(float Q, float amplitude, float frequency) override {
        juce::ignoreUnused(amplitude);
        const auto sampleRate = static_cast<float>(sampleRate_);    
        const auto w0 = 2.0f * static_cast<float>(M_PI) * frequency / sampleRate;
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
};