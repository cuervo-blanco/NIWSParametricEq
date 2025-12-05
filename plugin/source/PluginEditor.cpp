#include "SimpleParametricEq/PluginEditor.h"
#include "SimpleParametricEq/PluginProcessor.h"

namespace parametric_eq {
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(
    AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p) {
  juce::ignoreUnused(processorRef);
  setSize(1080, 450);
  startTimerHz(30);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0,0,0));

    if (spectrumMagnitudes_.empty()) {
        return;
    }

    auto bounds = getLocalBounds().toFloat().reduced(10.0f);

    g.setColour(juce::Colours::white);

    frequencyResponseGUI_.setBounds(bounds);
    frequencyResponseGUI_.paint(g, spectrumMagnitudes_);
}

void AudioPluginAudioProcessorEditor::resized() {}

void AudioPluginAudioProcessorEditor::timerCallback() {
    auto& analyzer = processorRef.getSpectrumAnalyzer();

    if (analyzer.isNewFFTReady()) {
        const auto& mags = analyzer.getMagnitudesDb();
        spectrumMagnitudes_ = mags;
        analyzer.clearNewFFTFlag();
        repaint();
    }
}

}  // namespace parametric_eq
