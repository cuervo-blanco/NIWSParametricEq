#pragma once

#include <cmath>
#include <juce_dsp/juce_dsp.h>

class NotchFilter {
public:
    NotchFilter() = default;
    ~NotchFilter() = default;
    void prepare(double sampleRate, double frequency, double Q);
    void reset();

    void processBlock(juce::AudioBuffer<float>& buffer);
    void processSample(float* inputSample);

    void setFrequency(double frequency);
    void setQ(double Q);
    void setAmplitude(float gainDb) {
        amplitude_ = std::sqrt(std::pow(10.0f, gainDb / 20.0f));
    }
private:
    void calculateCoefficients();

    double sampleRate_{44100.0};
    float amplitude_ = 1.0f;
    double frequency_ = 1000.0;
    double Q_ = 1.0;

    float a0;
    float a1;
    float a2;
    float b1;
    float b2;
    float b0{1.0f};

    float z1 = 0.0f;
    float z2 = 0.0f;
};