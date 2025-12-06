#pragma once

#include "FilterParameters.h"
class BandPassFilter : public BiquadFilter {
public:
    BandPassFilter() = default;
    ~BandPassFilter() override = default;
private:
    void calculateAndSetCoefficients(float Q, float amplitude) override {
        juce::ignoreUnused(amplitude);

        const auto w0 = static_cast<float>(2.0 * M_PI * frequency_ / sampleRate_);
        const auto cos_w = static_cast<float>(std::cos(w0));
        const auto alpha = std::sin(w0) / (2.0f * Q);

        float a0 = 1.f + alpha;

        float b0 = alpha;
        float b1 = 0.f;
        float b2 = -alpha;
        float a1 = -2.f * cos_w;
        float a2 = 1.f - alpha;

        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
        a1 /= a0;
        a2 /= a0;

        setCoefficients(b0, b1, b2, a0, a1, a2);
    }
};