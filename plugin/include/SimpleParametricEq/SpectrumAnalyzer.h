#pragma once
#include <juce_dsp/juce_dsp.h>
#include "utils/RingBuffer.h"

class SpectrumAnalyzer {
public:
    SpectrumAnalyzer(int fftOrder);

    void prepare(double sampleRate, int numInputChannels);
    void pushBlock(const juce::AudioBuffer<float>& buffer);

    bool isNewFFTReady() const noexcept { return newFFTReady_; }
    const std::vector<float>& getMagnitudesDb() const { return magnitudeDb_; }
    void clearNewFFTFlag() noexcept { newFFTReady_ = false; }

private:
    void performFFT();

    int fftOrder_;
    size_t fftSize_;

    juce::dsp::FFT fft_;
    juce::dsp::WindowingFunction<float> window_;

    RingBuffer ringBuffer_;
    int samplesSinceLastFFT_{0};

    std::vector<float> fftBuffer_; 
    std::vector<float> magnitudeDb_; 

    std::atomic<bool> newFFTReady_{false};
};