#pragma once

#include "FilterParameters.h"
class HighShelfFilter : public BiquadFilter {
public:
    HighShelfFilter() = default;
    ~HighShelfFilter() override = default;
private:
    void calculateAndSetCoefficients(float Q, float amplitude, float frequency) override {
        const auto sampleRate = static_cast<float>(sampleRate_);
        const auto w0 = 2.0f * static_cast<float>(M_PI) * frequency / sampleRate;
        const auto cos_w = static_cast<float>(std::cos(w0));
        const auto alpha = std::sin(w0) / (2.0f * Q);

        const auto A = amplitude;
        const auto twoSqrtAlpha = 2.f * std::sqrt(A) * alpha;

        float a0 = (A + 1.f) + (A - 1.f) * cos_w + twoSqrtAlpha;

        float b0 = A * ((A + 1.f) + (A - 1.f) * cos_w + twoSqrtAlpha);
        float b1 = -2.f * A * ((A - 1.f) + (A + 1.f) * cos_w);
        float b2 = A * ((A + 1.f) + (A - 1.f) * cos_w) - twoSqrtAlpha;
        float a1 = 2.f * ((A - 1.f) + (A + 1.f) * cos_w);
        float a2 = (A + 1.f) - (A - 1.f) * cos_w - twoSqrtAlpha;

        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
        a1 /= a0;
        a2 /= a0;

        setCoefficients(b0, b1, b2, a0, a1, a2);
    }
};