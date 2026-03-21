#pragma once

#include "PluginProcessor.h"
#include "FilterInspectorPanel.h"
#include "FrequencyResponseGUI.h"
#include "gui/FrequencyAxis.h"
#include "gui/BandComponent.h"
namespace parametric_eq {
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
  explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&);
  ~AudioPluginAudioProcessorEditor() override;

  void paint(juce::Graphics&) override;
  void resized() override;

private:
  using FilterSelection = FilterInspectorPanel::Selection;

  void timerCallback() override;
  void selectFilter(BandComponent& band, FilterSelection selection);

  AudioPluginAudioProcessor& processorRef;
  FrequencyAxis frequencyAxis_;
  FrequencyResponseGUI frequencyResponseGUI_;
  FilterInspectorPanel filterInspectorPanel_;

  BandComponent peakBand0_;
  BandComponent peakBand1_;
  BandComponent peakBand2_;
  BandComponent peakBand3_;

  BandComponent lowPassBand_;
  BandComponent highPassBand_;

  BandComponent highShelfBand_;
  BandComponent lowShelfBand_;
  BandComponent* selectedBand_ { nullptr };

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
}  // namespace parametric_eq
