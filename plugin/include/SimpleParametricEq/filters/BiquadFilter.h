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

            for (int ch = 0; ch < numChannels; ++ch) {
                auto* channelData = buffer.getWritePointer(ch);
                auto x = channelData[n];

                auto& z1 = z1_[static_cast<size_t>(ch)];
                auto& z2 = z2_[static_cast<size_t>(ch)];

                const auto y = b0_ * x + z1;
                z1 = b1_ * x - a1_ * y + z2;
                z2 = b2_ * x - a2_ * y;

                channelData[n] = y;
            }
        }
    }

protected:
    static constexpr float EPSILON = 1e-3f;

    virtual void updateSmoothedParameters() = 0;

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
};
