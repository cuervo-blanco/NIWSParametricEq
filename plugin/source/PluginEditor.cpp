#include "NIWSParametricEq/PluginEditor.h"
#include "NIWSParametricEq/PluginProcessor.h"
#include "NIWSParametricEq/ParametricEq.h"
#include "NIWSParametricEq/Parameters.h"

namespace parametric_eq {
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(
    AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p),
    peakBand0_ (processorRef.getParameters().peakFilters[0]->base.frequency,
                processorRef.getParameters().peakFilters[0]->gain,
                frequencyAxis_,
                BandComponent::BandType::Peak)
{
    setSize(1080, 450);
    startTimerHz(30);

    addAndMakeVisible(frequencyResponseGUI_);
    addAndMakeVisible(frequencyAxis_);
    addAndMakeVisible(peakBand0_);

    auto bands = processorRef.getParametricEq().getBands();

    frequencyAxis_.setInterceptsMouseClicks(false, false);
    frequencyAxis_.setDbRange(-40.0f, 40.0f);
    frequencyAxis_.setReferenceBands(bands);

    frequencyResponseGUI_.setInterceptsMouseClicks(false, false);
    frequencyResponseGUI_.setSampleRate(processorRef.getSampleRate());

    peakBand0_.setDbRange(-40.0f, 40.0f);
    peakBand0_.updateFromParameters();
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0,0,0));
}

void AudioPluginAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds().reduced(10); 
    frequencyResponseGUI_.setBounds(bounds);
    frequencyAxis_.setBounds(bounds);
    peakBand0_.setBounds(bounds);

}

void AudioPluginAudioProcessorEditor::timerCallback() {
    auto& analyzer = processorRef.getSpectrumAnalyzer();

    if (analyzer.isNewFFTReady()) {
        const auto& mags = analyzer.getMagnitudesDb();
        frequencyResponseGUI_.setMagnitudes(mags);
        analyzer.clearNewFFTFlag();
    }

    peakBand0_.updateFromParameters();
}

}  // namespace parametric_eq
