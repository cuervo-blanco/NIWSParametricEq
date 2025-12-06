#pragma once

#include "FilterParameters.h"

class PeakFilter : public BiquadFilter {
public:
    PeakFilter() = default;
    ~PeakFilter() override = default;
private:
    void calculateAndSetCoefficients(float Q, float amplitude) override {
        const auto w0 = static_cast<float>(2.0 * M_PI * frequency_ / sampleRate_);
        const auto cos_w = static_cast<float>(std::cos(w0));
        const auto alpha = std::sin(w0) / (2.0f * Q);

        const auto A = amplitude;

        float a0 = 1.0f + (alpha / A);

        float b0 = 1.0f + (alpha * A);
        float b1 = -2.0f * cos_w;
        float b2 = 1.0f - (alpha * A);
        float a1 = -2.0f * cos_w;
        float a2 = 1.0f - (alpha / A);

        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
        a1 /= a0;
        a2 /= a0;

        setCoefficients(b0, b1, b2, a0, a1, a2);
    }
};