#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "BiquadFilter.h"

struct BaseParameters {
    juce::AudioParameterFloat& frequency; 
    juce::AudioParameterFloat& qFactor;
    juce::AudioParameterChoice& slope; 
    juce::AudioParameterBool& bypassed; 
};

struct BoostCutParameters {
    BaseParameters base;
    juce::AudioParameterFloat& gain;
};

class FilterParameters : public BiquadFilter {
protected:
    double frequency_{1000.0};
    double QRaw_{1.0};
    float gainDbRaw_{0.0f};

    juce::SmoothedValue<float> qSmoothed_;
    juce::SmoothedValue<float> gainSmoothed_;

    float lastQ_{1.0f};
    float lastA_{1.0f};

    bool coeffsDirty_{true};

    virtual void calculateAndSetCoefficients(float Q, float amplitude) = 0;

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

public:
    FilterParameters() = default;
    ~FilterParameters() override = default;

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
}; 