#pragma once

#include "FilterParameters.h"
class HighShelfFilter : public BiquadFilter {
public:
    HighShelfFilter() = default;
    ~HighShelfFilter() override = default;
private:
    void calculateAndSetCoefficients(float Q, float A, float frequency) override {
        const auto sampleRate = static_cast<float>(sampleRate_);
        const auto w0 = 2.0f * static_cast<float>(M_PI) * frequency / sampleRate;

        const auto cos_w = static_cast<float>(std::cos(w0));
        const auto sin_w = static_cast<float>(std::sin(w0));

        auto S = shelfSlopeS_from_Q(Q, A);

        const float alpha = (sin_w * 0.5f) 
            * std::sqrt((A + 1.0f / A) * (1.0f / S - 1.0f) + 2.0f);


        const auto twoSqrtAlpha = 2.f * std::sqrt(A) * alpha;

        float b0 = A * ((A + 1.f) + (A - 1.f) * cos_w + twoSqrtAlpha);
        float b1 = -2.f * A * ((A - 1.f) + (A + 1.f) * cos_w);
        float b2 = A * ((A + 1.f) + (A - 1.f) * cos_w - twoSqrtAlpha);

        float a0 = (A + 1.f) - (A - 1.f) * cos_w + twoSqrtAlpha;
        float a1 = 2.f * ((A - 1.f) - (A + 1.f) * cos_w);
        float a2 = (A + 1.f) - (A - 1.f) * cos_w - twoSqrtAlpha;

        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
        a1 /= a0;
        a2 /= a0;
        a0 /= a0;

        setCoefficients(b0, b1, b2, 1.0f, a1, a2);
    }
};