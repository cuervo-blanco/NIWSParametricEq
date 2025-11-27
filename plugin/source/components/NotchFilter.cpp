#include "SimpleParametricEq/components/NotchFilter.h"

void NotchFilter::prepare(double sampleRate, double frequency, double Q) {
    sampleRate_ = sampleRate;
    frequency_ = frequency;
    Q_ = Q;
    calculateCoefficients();
    reset();
}

void NotchFilter::reset() {
    z1 = 0.0f;
    z2 = 0.0f;
}

void NotchFilter::calculateCoefficients() {
    auto w0 = 2.0 * M_PI * frequency_ / sampleRate_;
    auto alpha = static_cast<float>(std::sin(w0) / (2.0 * Q_));

    auto cos_w0 = static_cast<float>(std::cos(w0));

    b1 = -2.0f * cos_w0;
    b2 = 1.0f;
    a0 = 1.0f + alpha;
    a1 = -2.0f * cos_w0;
    a2 = 1.0f - alpha;

    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
}

void NotchFilter::processBlock(juce::AudioBuffer<float>& buffer) {
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            float inputSample = channelData[sample];
            processSample(&inputSample);
            channelData[sample] = inputSample * amplitude_;
        }
    }
}

void NotchFilter::processSample(float* x) {
    float x_n = *x;
    float y_n = b0 * x_n + z1;
    z1 = b1 * x_n - a1 * y_n + z2;
    z2 = b2 * x_n - a2 * y_n;
    *x = y_n * amplitude_;
}

void NotchFilter::setFrequency(double frequency) {
    frequency_ = frequency;
    calculateCoefficients();
}

void NotchFilter::setQ(double Q) {
    Q_ = Q;
    calculateCoefficients();
}