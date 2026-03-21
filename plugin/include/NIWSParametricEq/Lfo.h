#pragma once

#include <array>
#include <cmath>
#include <juce_dsp/juce_dsp.h>

namespace parametric_eq {
class Lfo {
public:
    enum class Waveform : std::size_t {
        Sine = 0,
        Triangle,
        Square,
        Saw
    };

    Lfo() {
        using Osc = juce::dsp::Oscillator<float>;

        oscillators_[static_cast<std::size_t>(Waveform::Sine)] =
            Osc { [] (float phase) { return std::sin(phase); } };

        oscillators_[static_cast<std::size_t>(Waveform::Triangle)] =
            Osc { [] (float phase) { return triangle(phase); } };

        oscillators_[static_cast<std::size_t>(Waveform::Square)] =
            Osc { [] (float phase) { return square(phase); } };

        oscillators_[static_cast<std::size_t>(Waveform::Saw)] =
            Osc { [] (float phase) { return saw(phase); } };

        for (auto& osc : oscillators_) {
            osc.setFrequency(1.0f, true);
        }
    }

    void prepare(const juce::dsp::ProcessSpec& spec) {
        sampleRate_ = spec.sampleRate;

        for (auto& osc : oscillators_)
            osc.prepare(spec);

        reset();
    }

    void reset() noexcept {
        for (auto& osc : oscillators_) {
            osc.reset();
        }
    }

    void setWaveform(Waveform wf) noexcept {
        currentWaveform_ = wf;
    }

    void setFrequency(float hz) noexcept {
        frequencyHz_ = hz;
        for (auto& osc : oscillators_) {
            osc.setFrequency(hz, true);
        }
    }

    float getFrequency() const noexcept {
        return frequencyHz_;
    }

    float getNextSample() noexcept {
        auto& osc = oscillators_[static_cast<std::size_t>(currentWaveform_)];
        return osc.processSample(0.0f);
    }

    void fillBuffer(juce::AudioBuffer<float>& buffer, int channel = 0) noexcept {
        jassert(channel >= 0 && channel < buffer.getNumChannels());

        auto* writePtr = buffer.getWritePointer(channel);
        const int numSamples = buffer.getNumSamples();

        for (int i = 0; i < numSamples; ++i) {
            writePtr[i] = getNextSample();
        }
    }

    float getNextUnipolarSample() noexcept {
        return 0.5f * (getNextSample() + 1.0f);
    }

    float advanceAndGetLastSample(int numSamples) noexcept {
        float value = 0.0f;

        for (int sample = 0; sample < numSamples; ++sample) {
            value = getNextSample();
        }

        return value;
    }

    float advanceAndGetLastUnipolarSample(int numSamples) noexcept {
        float value = 0.0f;

        for (int sample = 0; sample < numSamples; ++sample) {
            value = getNextUnipolarSample();
        }

        return value;
    }

private:
    static float triangle(float phase) noexcept {
        const float twoPi = juce::MathConstants<float>::twoPi;
        const float ft = phase / twoPi;
        return (4.0f * std::abs(ft - std::floor(ft + 0.5f))) - 1.0f;
    }

    static float square(float phase) noexcept {
        const float pi = juce::MathConstants<float>::pi;
        return (phase < pi) ? 1.0f : -1.0f;
    }

    static float saw(float phase) noexcept {
        const float twoPi = juce::MathConstants<float>::twoPi;
        const float norm = phase / twoPi;       // [0, 1)
        return 2.0f * norm - 1.0f;             // [-1, 1)
    }

    std::array<juce::dsp::Oscillator<float>, 4> oscillators_;
    Waveform currentWaveform_ { Waveform::Sine };

    double sampleRate_ { 44100.0 };
    float  frequencyHz_ { 1.0f };

};
} // namespace parametric_eq
