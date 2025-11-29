#include "SimpleParametricEq/Parameters.h"
#include "SimpleParametricEq/ParametricEq.h"
#include "SimpleParametricEq/filters/FilterParameters.h"
namespace parametric_eq {
namespace {
struct Identifier {
    juce::String id;
    juce::String name;
    int versionHint = 1;
};

auto& addParameterToProcessor(juce::AudioProcessor& processor, auto parameter) {
  auto& result = *parameter;
  processor.addParameter(parameter.release());
  return result;
}

std::array<juce::AudioParameterFloat*, ParametricEq::NUM_PEAKS> createPeakFreqsParameters(juce::AudioProcessor& processor) {
    std::array<juce::AudioParameterFloat*, ParametricEq::NUM_PEAKS> parameters{};
    constexpr auto versionHint = 1;
    for (size_t i = 0; i < ParametricEq::NUM_PEAKS; i++) {
        juce::String id = "peakFreq" + juce::String(i + 1);
        juce::String name = "Peak " + juce::String(i + 1) + " Frequency";
        parameters[i] = &addParameterToProcessor(
            processor,
            std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID{id, versionHint}, name,
                juce::NormalisableRange<float>{20.f, 20000.f, 1.f, 0.4f}, 
                static_cast<float>(ParametricEq::DEFAULT_FREQS[i]),
                juce::AudioParameterFloatAttributes{}.withLabel("Hz")));
    }
    return parameters;
}

std::array<juce::AudioParameterFloat*, ParametricEq::NUM_PEAKS> createPeakQsParameters(juce::AudioProcessor& processor) {
    std::array<juce::AudioParameterFloat*, ParametricEq::NUM_PEAKS> parameters{};
    constexpr auto versionHint = 1;
    for (size_t i = 0; i < ParametricEq::NUM_PEAKS; i++) {
        juce::String id = "peakQ" + juce::String(i + 1);
        juce::String name = "Peak " + juce::String(i + 1) + " Q-Factor";
        parameters[i] = &addParameterToProcessor(
            processor,
            std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID{id, versionHint}, name,
                juce::NormalisableRange<float>{0.1f, 100.f, 0.1f, 0.5f}, 
                1.f,
                juce::AudioParameterFloatAttributes{}.withLabel("")));
    }
    return parameters;
}

std::array<juce::AudioParameterFloat*, ParametricEq::NUM_PEAKS> createPeakGainsParameters(juce::AudioProcessor& processor) {
    std::array<juce::AudioParameterFloat*, ParametricEq::NUM_PEAKS> parameters{};
    constexpr auto versionHint = 1;
    for (size_t i = 0; i < ParametricEq::NUM_PEAKS; i++) {
        juce::String id = "peakGain" + juce::String(i + 1);
        juce::String name = "Peak " + juce::String(i + 1) + " Gain";
        parameters[i] = &addParameterToProcessor(
            processor,
            std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID{id, versionHint}, name,
                juce::NormalisableRange<float>{-48.f, 48.f, 0.1f, 0.5f}, 
                0.f,
                juce::AudioParameterFloatAttributes{}.withLabel("dB")));
    }
    return parameters;
}

juce::AudioParameterFloat* createFrequencyParameter(juce::AudioProcessor& processor, Identifier identifier){
    return &addParameterToProcessor(
        processor,
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{identifier.id, identifier.versionHint}, identifier.name,
            juce::NormalisableRange<float>{20.f, 20000.f, 1.f, 0.4f}, 
            80.0f,
            juce::AudioParameterFloatAttributes{}.withLabel("Hz")));
}

juce::AudioParameterFloat* createQParameter(juce::AudioProcessor& processor, Identifier identifier){
    return &addParameterToProcessor(
        processor,
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{identifier.id, identifier.versionHint}, identifier.name,
            juce::NormalisableRange<float>{0.1f, 100.f, 0.1f, 0.5f}, 
            1.f,
            juce::AudioParameterFloatAttributes{}.withLabel("")));
}

juce::AudioParameterFloat* createGainParameter(juce::AudioProcessor& processor, Identifier identifier){
    return &addParameterToProcessor(
        processor,
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{identifier.id, identifier.versionHint}, identifier.name,
            juce::NormalisableRange<float>{-48.f, 48.f, 0.1f, 0.5f}, 
            0.f,
            juce::AudioParameterFloatAttributes{}.withLabel("dB")));

}

LowShelfParameters createLowShelfParameters(juce::AudioProcessor& processor) {
    Identifier frequencyIdentifier = {"lowShelfFrequency", "Low Shelf Frequency", 1};
    Identifier qIdentifier = {"lowShelfQ", "Low Shelf Q-Factor", 1};
    Identifier gainIdentifier = {"lowShelfGain", "Low Shelf Gain", 1};
    
    auto frequency = createFrequencyParameter(processor, frequencyIdentifier);
    auto q = createQParameter(processor, qIdentifier);
    auto gain = createGainParameter(processor, gainIdentifier);

    LowShelfParameters parameters = { *frequency, *q, *gain };

    return parameters;
}

} // namespace
Parameters::Parameters(juce::AudioProcessor& processor)
    : peakFreqs{createPeakFreqsParameters(processor)},
      peakQs{createPeakQsParameters(processor)},
      peakGains{createPeakGainsParameters(processor)},
      lowShelfParameters{createLowShelfParameters(processor)} {}
}  // namespace parametric_eq
