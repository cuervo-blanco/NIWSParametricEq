#include "SimpleParametricEq/PluginEditor.h"
#include "SimpleParametricEq/PluginProcessor.h"
#include "SimpleParametricEq/ParametricEq.h"
#include "SimpleParametricEq/Parameters.h"

namespace parametric_eq {
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(
    AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p),
    peakBand0_(processorRef.getParameters().peakFilters[0]->base.frequency,
               processorRef.getParameters().peakFilters[0]->gain,
               frequencyAxis_,
               BandComponent::BandType::Peak),
    peakBand1_(processorRef.getParameters().peakFilters[1]->base.frequency,
               processorRef.getParameters().peakFilters[1]->gain,
               frequencyAxis_,
               BandComponent::BandType::Peak),
    peakBand2_(processorRef.getParameters().peakFilters[2]->base.frequency,
               processorRef.getParameters().peakFilters[2]->gain,
               frequencyAxis_,
               BandComponent::BandType::Peak),
    lowPassBand_(processorRef.getParameters().lowPassParameters.frequency,
                 processorRef.getParameters().lowPassParameters.qFactor,
                 frequencyAxis_,
                 BandComponent::BandType::LowPass),
    highPassBand_(processorRef.getParameters().highPassParameters.frequency,
                  processorRef.getParameters().highPassParameters.qFactor,
                  frequencyAxis_,
                  BandComponent::BandType::HighPass)
{
    setSize(1080, 450);
    startTimerHz(30);

    addAndMakeVisible(frequencyResponseGUI_);
    addAndMakeVisible(frequencyAxis_);
    addAndMakeVisible(peakBand0_);
    addAndMakeVisible(peakBand1_);
    addAndMakeVisible(peakBand2_);
    addAndMakeVisible(lowPassBand_);
    addAndMakeVisible(highPassBand_);

    auto bands = processorRef.getParametricEq().getBands();

    frequencyAxis_.setInterceptsMouseClicks(false, false);
    frequencyAxis_.setDbRange(-40.0f, 40.0f);
    frequencyAxis_.setReferenceBands(bands);

    frequencyResponseGUI_.setInterceptsMouseClicks(false, false);
    frequencyResponseGUI_.setSampleRate(processorRef.getSampleRate());

    peakBand0_.setDbRange(-40.0f, 40.0f);
    peakBand0_.updateFromParameters();
    peakBand1_.setDbRange(-40.0f, 40.0f);
    peakBand1_.updateFromParameters();
    peakBand2_.setDbRange(-40.0f, 40.0f);
    peakBand2_.updateFromParameters();

    lowPassBand_.setDbRange(-40.0f, 40.0f);
    lowPassBand_.updateFromParameters();
    highPassBand_.setDbRange(-40.0f, 40.0f);
    highPassBand_.updateFromParameters();
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0,50,94));
}

void AudioPluginAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds().reduced(10); 
    frequencyResponseGUI_.setBounds(bounds);
    frequencyAxis_.setBounds(bounds);
    peakBand0_.setBounds(bounds);
    peakBand1_.setBounds(bounds);
    peakBand2_.setBounds(bounds);
    lowPassBand_.setBounds(bounds);
    highPassBand_.setBounds(bounds);
}

void AudioPluginAudioProcessorEditor::timerCallback() {
    auto& analyzer = processorRef.getSpectrumAnalyzer();

    if (analyzer.isNewFFTReady()) {
        const auto& mags = analyzer.getMagnitudesDb();
        frequencyResponseGUI_.setMagnitudes(mags);
        analyzer.clearNewFFTFlag();
    }

    peakBand0_.updateFromParameters();
    peakBand1_.updateFromParameters();
    peakBand2_.updateFromParameters();
    lowPassBand_.updateFromParameters();
    highPassBand_.updateFromParameters();
}

}  // namespace parametric_eq
