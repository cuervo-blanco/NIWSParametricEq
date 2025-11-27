#pragma once

#include <cmath>
#include <juce_dsp/juce_dsp.h>

class PeakFilter {
public:
    static constexpr float EPSILON = 1e-3f;

    PeakFilter() = default;
    ~PeakFilter() = default;
    void prepare(double sampleRate, double frequency, double Q);
    void reset();

    void processBlock(juce::AudioBuffer<float>& buffer);

    void setFrequency(double frequency);
    void setQ(double Q);
    void setAmplitude(float gainDb);
private:
    void calculateCoefficients(double Q, float amplitude);

    double sampleRate_{44100.0};
    double frequency_ = 1000.0;
    double QRaw_ = 1.0;
    float gainDbRaw_ = 1.0f;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> qSmoothed_;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gainSmoothed_; // linear A

    float a0;
    float a1;
    float a2;
    float b0;
    float b1;
    float b2;

    std::vector<float> z1_;
    std::vector<float> z2_;

    bool coeffsDirty_ = true;
};