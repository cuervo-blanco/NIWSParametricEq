#pragma once
#include <juce_dsp/juce_dsp.h>
#include "filters/BiquadFilter.h"
#include <ranges>

namespace parametric_eq {
class BypassTransitioner {
public:
  explicit BypassTransitioner(double crossfadeLengthSecondsValue = 0.01)
      : crossfadeLengthSeconds{crossfadeLengthSecondsValue} {
    jassert(0.0 < crossfadeLengthSeconds);
  }

  void prepare(const juce::dsp::ProcessSpec& spec) {
    sampleRateHz = spec.sampleRate;
    dryBuffer.setSize(static_cast<int>(spec.numChannels),
                      static_cast<int>(spec.maximumBlockSize));
    //dryGain.reset(spec.sampleRate, crossfadeLengthSeconds);
    //wetGain.reset(spec.sampleRate, crossfadeLengthSeconds);
  }

  void setBypass(bool bypass) noexcept {
    const auto current = dryGain.getCurrentValue();
    const auto target = bypass ? 1.0f : 0.0f;
    const auto diff = std::abs(target - current);
    if (diff < 1e-3f) {
      return;
    }
    const auto duration = crossfadeLengthSeconds * static_cast<double>(std::abs(target - current));

    dryGain.reset(sampleRateHz, duration);
    wetGain.reset(sampleRateHz, duration);

    dryGain.setCurrentAndTargetValue(current);
    dryGain.setTargetValue(target);

    wetGain.setCurrentAndTargetValue(1.0f - current);
    wetGain.setTargetValue(1.0f - target);
  }

  void setBypassForced(bool bypass) noexcept {
    dryGain.setCurrentAndTargetValue(bypass ? 1.0f : 0.0f);
    wetGain.setCurrentAndTargetValue(1.0f - dryGain.getTargetValue());
  }

  [[nodiscard]] bool isTransitioning() const noexcept {
    return dryGain.isSmoothing() || wetGain.isSmoothing();
  }

  void setDryBuffer(const juce::AudioBuffer<float>& buffer) noexcept {
    auto totalNumSamples = buffer.getNumSamples();
    auto totalNumChannels = buffer.getNumChannels();

    jassert(totalNumSamples <= dryBuffer.getNumSamples());
    jassert(totalNumChannels <= dryBuffer.getNumChannels());

    for (int ch = 0; ch < totalNumChannels; ch++) {
      dryBuffer.copyFrom(ch, 0, buffer, ch, 0, totalNumSamples);
    };

    dryGain.applyGain(dryBuffer, totalNumSamples);
  }

  void mixToWetBuffer(juce::AudioBuffer<float>& buffer) noexcept {
    auto totalNumSamples = buffer.getNumSamples();
    auto totalNumChannels = buffer.getNumChannels();

    jassert(totalNumSamples <= dryBuffer.getNumSamples());
    jassert(totalNumChannels <= dryBuffer.getNumChannels());

    wetGain.applyGain(buffer, totalNumSamples);
    for (int ch = 0; ch < totalNumChannels; ch++) {
      buffer.addFrom(ch, 0, dryBuffer, ch, 0, totalNumSamples);
    };
  }

  void reset() noexcept {
    setBypassForced(false);
    dryBuffer.clear();
  }

private:
  double crossfadeLengthSeconds = 0.0;
  double sampleRateHz = 0.0;
  juce::LinearSmoothedValue<float> dryGain{0.f};
  juce::LinearSmoothedValue<float> wetGain{1.f};
  juce::AudioBuffer<float> dryBuffer;
};
}  // namespace parametric_eq
