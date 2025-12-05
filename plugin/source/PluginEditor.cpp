#include "SimpleParametricEq/PluginEditor.h"
#include "SimpleParametricEq/PluginProcessor.h"

namespace parametric_eq {
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(
    AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p) 
{
    setSize(1080, 450);
    startTimerHz(30);

    addAndMakeVisible(frequencyResponseGUI_);
    addAndMakeVisible(frequencyAxis_);

    frequencyAxis_.setInterceptsMouseClicks(false, false);
    frequencyAxis_.setDbRange(-40.0f, 40.0f);

    frequencyResponseGUI_.setInterceptsMouseClicks(false, false);
    frequencyResponseGUI_.setSampleRate(processorRef.getSampleRate());
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0,0,0));
}

void AudioPluginAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds().reduced(10); 
    frequencyResponseGUI_.setBounds(bounds);
    frequencyAxis_.setBounds(bounds);
}

void AudioPluginAudioProcessorEditor::timerCallback() {
    auto& analyzer = processorRef.getSpectrumAnalyzer();

    if (analyzer.isNewFFTReady()) {
        const auto& mags = analyzer.getMagnitudesDb();
        frequencyResponseGUI_.setMagnitudes(mags);
        analyzer.clearNewFFTFlag();
    }
}

}  // namespace parametric_eq
