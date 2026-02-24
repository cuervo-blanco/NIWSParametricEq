#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>

class RingBuffer {
private:
    juce::AudioBuffer<float> buffer_;
    int capacity_{0};
    int writeIndex_{0};
    int wrapMask_{0};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RingBuffer)

    double getPowerOfTwo(int n) {
        auto power = std::ceil(std::log2(n));
        return std::pow(2.0, power);
    }
public:
    RingBuffer() = default;
    RingBuffer(int capacity, int numChannels) {
        reset(capacity, numChannels);
    }

    ~RingBuffer() = default;

    void reset(int capacity, int numChannels) {
        capacity_ = static_cast<int>(getPowerOfTwo(capacity));
        buffer_.setSize(numChannels, capacity_);
        wrapMask_ = capacity_ - 1;
        clear();
    }

    void clear() {
        buffer_.clear();
        writeIndex_ = 0;
    }

    [[nodiscard]] int getCapacity() const noexcept { return capacity_; }
    [[nodiscard]] int getNumChannels() const noexcept { return buffer_.getNumChannels(); }

    void writeFrame(const float* samples, int numChannels) {
        jassert(numChannels == buffer_.getNumChannels());
        for (int ch = 0; ch < numChannels; ++ch) {
            buffer_.setSample(ch, writeIndex_, samples[ch]);
        }
        writeIndex_ = (writeIndex_ + 1) & wrapMask_;
    }

    void writeBlock(const juce::AudioBuffer<float>& src) {
        const auto numChannels = juce::jmin(src.getNumChannels(), buffer_.getNumChannels());
        const auto numSamples  = src.getNumSamples();
        std::vector<float> frame(static_cast<size_t>(numChannels));

        for (int n = 0; n < numSamples; ++n) {
            for (int ch = 0; ch < numChannels; ++ch) {
                frame[static_cast<uint8_t>(ch)] = src.getReadPointer(ch)[n];
            }
            writeFrame(frame.data(), numChannels);
        }
    }

    [[nodiscard]] float readSampleAtDelay(int channel, int delayInSamples) const {
        int idx = writeIndex_ - 1 - delayInSamples;
        idx &= wrapMask_; 
        return buffer_.getSample(channel, idx);
    }

    void copyMostRecentBlock(int channel, float* dest, int numSamples) const {
        jassert(numSamples <= capacity_);
        jassert(channel >= 0 && channel < buffer_.getNumChannels());

        const auto* src = buffer_.getReadPointer(channel);

        auto startIdx = (writeIndex_ - numSamples) & wrapMask_;
        auto firstPart = static_cast<size_t>(juce::jmin(numSamples, capacity_ - startIdx));
        auto secondPart = static_cast<size_t>(numSamples) - firstPart;

        std::memcpy(dest, src + startIdx, sizeof(float) * firstPart);

        if (secondPart > 0) {
            std::memcpy(dest + firstPart, src, sizeof(float) * secondPart);
        }
    }

    void copyMostRecentSamplesMono(float* dest, int numSamples) const {
        jassert(numSamples <= capacity_);
        const auto numChannels = buffer_.getNumChannels();
        for (int n = 0; n < numSamples; ++n) {
            auto idx = (writeIndex_ - numSamples + n) & wrapMask_;
            auto sum = 0.0f;
            for (int ch = 0; ch < numChannels; ++ch) {
                sum += buffer_.getSample(ch, idx);
            }
            dest[n] = sum / static_cast<float>(numChannels);
        }
    }
};
