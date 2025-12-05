#pragma once

#include "PluginProcessor.h"
#include "FrequencyResponseGUI.h"
#include "gui/FrequencyAxis.h"

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

  FrequencyAxis frequencyAxis_;
  FrequencyResponseGUI frequencyResponseGUI_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
}  // namespace parametric_eq
