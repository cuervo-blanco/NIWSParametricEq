#pragma once

#include "PluginProcessor.h"

namespace parametric_eq {
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
  explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&);
  ~AudioPluginAudioProcessorEditor() override;

  void paint(juce::Graphics&) override;
  void resized() override;

private:
  void timerCallback() override;

  AudioPluginAudioProcessor& processorRef;

  std::vector<float> spectrumMagnitudes_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
}  // namespace parametric_eq
