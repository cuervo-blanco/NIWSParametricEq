#include "SimpleParametricEq/SpectrumAnalyzer.h"

SpectrumAnalyzer::SpectrumAnalyzer(int fftOrder)
    : fftOrder_(fftOrder),
    fftSize_(size_t{1} << fftOrder_),
    fft_(fftOrder_),
    window_(fftSize_, juce::dsp::WindowingFunction<float>::hann),
    fftBuffer_(fftSize_ * 2, 0.0f),
    magnitudeDb_(fftSize_ / 2, -100.0f) {}

void SpectrumAnalyzer::prepare(double sampleRate, int numInputChannels) {
    juce::ignoreUnused(sampleRate);
    ringBuffer_.reset(static_cast<int>(fftSize_ * 4), numInputChannels); 
    newFFTReady_ = false;
}

void SpectrumAnalyzer::pushBlock(const juce::AudioBuffer<float>& buffer) {
    ringBuffer_.writeBlock(buffer);

    samplesSinceLastFFT_ += buffer.getNumSamples();
    const auto hopSize = static_cast<int>(fftSize_);

    while (samplesSinceLastFFT_ >= hopSize) {
        performFFT();
        samplesSinceLastFFT_ -= hopSize;
    }
}

void SpectrumAnalyzer::performFFT() {
    ringBuffer_.copyMostRecentSamplesMono(fftBuffer_.data(), static_cast<int>(fftSize_));

    window_.multiplyWithWindowingTable(fftBuffer_.data(), fftSize_);

    std::fill(fftBuffer_.begin() + static_cast<int>(fftSize_), fftBuffer_.end(), 0.0f);

    fft_.performRealOnlyForwardTransform(fftBuffer_.data());

    const auto numBins = fftSize_ / 2;
    for (uint32_t bin = 0; bin < numBins; ++bin) {
        const auto real = fftBuffer_[2 * bin];
        const auto imag = fftBuffer_[2 * bin + 1];
        const auto mag  = std::sqrt(real * real + imag * imag);

        magnitudeDb_[bin] = juce::Decibels::gainToDecibels(mag, -120.0f);
    }

    newFFTReady_ = true;
}