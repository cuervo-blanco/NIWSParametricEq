#include "NIWSParametricEq/PluginEditor.h"
#include "NIWSParametricEq/PluginProcessor.h"
#include "NIWSParametricEq/ParametricEq.h"
#include "NIWSParametricEq/Parameters.h"

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
    peakBand3_(processorRef.getParameters().peakFilters[3]->base.frequency,
               processorRef.getParameters().peakFilters[3]->gain,
               frequencyAxis_,
               BandComponent::BandType::Peak),
    lowPassBand_(processorRef.getParameters().lowPassParameters.frequency,
                 processorRef.getParameters().lowPassParameters.qFactor,
                 frequencyAxis_,
                 BandComponent::BandType::LowPass),
    highPassBand_(processorRef.getParameters().highPassParameters.frequency,
                  processorRef.getParameters().highPassParameters.qFactor,
                  frequencyAxis_,
                  BandComponent::BandType::HighPass),
    highShelfBand_(processorRef.getParameters().highShelfParameters.base.frequency,
                  processorRef.getParameters().highShelfParameters.gain,
                  frequencyAxis_,
                  BandComponent::BandType::HighShelf),
    lowShelfBand_(processorRef.getParameters().lowShelfParameters.base.frequency,
                  processorRef.getParameters().lowShelfParameters.gain,
                  frequencyAxis_,
                  BandComponent::BandType::LowShelf)
{
    setSize(1080, 450);
    startTimerHz(30);

    addAndMakeVisible(frequencyResponseGUI_);
    addAndMakeVisible(frequencyAxis_);
    addChildComponent(filterInspectorPanel_);
    addAndMakeVisible(peakBand0_);
    addAndMakeVisible(peakBand1_);
    addAndMakeVisible(peakBand2_);
    addAndMakeVisible(peakBand3_);
    addAndMakeVisible(lowPassBand_);
    addAndMakeVisible(highPassBand_);
    addAndMakeVisible(highShelfBand_);
    addAndMakeVisible(lowShelfBand_);

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
    peakBand3_.setDbRange(-40.0f, 40.0f);
    peakBand3_.updateFromParameters();

    lowPassBand_.setDbRange(-40.0f, 40.0f);
    lowPassBand_.updateFromParameters();
    highPassBand_.setDbRange(-40.0f, 40.0f);
    highPassBand_.updateFromParameters();

    highShelfBand_.setDbRange(-40.0f, 40.0f);
    highShelfBand_.updateFromParameters();
    lowShelfBand_.setDbRange(-40.0f, 40.0f);
    lowShelfBand_.updateFromParameters();

    peakBand0_.setInteractionCallback([this]() {
        auto& peak = *processorRef.getParameters().peakFilters[0];
        selectFilter(peakBand0_, {"Peak 1", &peak.base, &peak.gain, &peak.lfo});
    });
    peakBand1_.setInteractionCallback([this]() {
        auto& peak = *processorRef.getParameters().peakFilters[1];
        selectFilter(peakBand1_, {"Peak 2", &peak.base, &peak.gain, &peak.lfo});
    });
    peakBand2_.setInteractionCallback([this]() {
        auto& peak = *processorRef.getParameters().peakFilters[2];
        selectFilter(peakBand2_, {"Peak 3", &peak.base, &peak.gain, &peak.lfo});
    });
    peakBand3_.setInteractionCallback([this]() {
        auto& peak = *processorRef.getParameters().peakFilters[3];
        selectFilter(peakBand3_, {"Peak 4", &peak.base, &peak.gain, &peak.lfo});
    });
    lowPassBand_.setInteractionCallback([this]() {
        auto& parameters = processorRef.getParameters().lowPassParameters;
        selectFilter(lowPassBand_, {"Low Pass", &parameters, nullptr, nullptr});
    });
    highPassBand_.setInteractionCallback([this]() {
        auto& parameters = processorRef.getParameters().highPassParameters;
        selectFilter(highPassBand_, {"High Pass", &parameters, nullptr, nullptr});
    });
    highShelfBand_.setInteractionCallback([this]() {
        auto& parameters = processorRef.getParameters().highShelfParameters;
        selectFilter(highShelfBand_, {"High Shelf", &parameters.base, &parameters.gain, &parameters.lfo});
    });
    lowShelfBand_.setInteractionCallback([this]() {
        auto& parameters = processorRef.getParameters().lowShelfParameters;
        selectFilter(lowShelfBand_, {"Low Shelf", &parameters.base, &parameters.gain, &parameters.lfo});
    });
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0,0,0));
}

void AudioPluginAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds().reduced(10); 
    frequencyResponseGUI_.setBounds(bounds);
    frequencyAxis_.setBounds(bounds);
    filterInspectorPanel_.setBounds(bounds.withTrimmedTop(bounds.getHeight() - 180));
    peakBand0_.setBounds(bounds);
    peakBand1_.setBounds(bounds);
    peakBand2_.setBounds(bounds);
    peakBand3_.setBounds(bounds);
    lowPassBand_.setBounds(bounds);
    highPassBand_.setBounds(bounds);
    highShelfBand_.setBounds(bounds);
    lowShelfBand_.setBounds(bounds);
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
    peakBand3_.updateFromParameters();
    lowPassBand_.updateFromParameters();
    highPassBand_.updateFromParameters();
    highShelfBand_.updateFromParameters();
    lowShelfBand_.updateFromParameters();
}

void AudioPluginAudioProcessorEditor::selectFilter(BandComponent& band, FilterSelection selection) {
    selectedBand_ = &band;
    filterInspectorPanel_.showSelection(selection);

    peakBand0_.setSelected(selectedBand_ == &peakBand0_);
    peakBand1_.setSelected(selectedBand_ == &peakBand1_);
    peakBand2_.setSelected(selectedBand_ == &peakBand2_);
    peakBand3_.setSelected(selectedBand_ == &peakBand3_);
    lowPassBand_.setSelected(selectedBand_ == &lowPassBand_);
    highPassBand_.setSelected(selectedBand_ == &highPassBand_);
    highShelfBand_.setSelected(selectedBand_ == &highShelfBand_);
    lowShelfBand_.setSelected(selectedBand_ == &lowShelfBand_);
}

}  // namespace parametric_eq
