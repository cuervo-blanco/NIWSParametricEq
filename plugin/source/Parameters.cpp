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

juce::AudioParameterFloat& createFrequencyParameter(juce::AudioProcessor& processor, Identifier identifier){
    return addParameterToProcessor(
        processor,
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{identifier.id, identifier.versionHint}, identifier.name,
            juce::NormalisableRange<float>{20.f, 20000.f, 1.f, 0.4f}, 
            80.0f,
            juce::AudioParameterFloatAttributes{}.withLabel("Hz")));
}

juce::AudioParameterFloat& createQParameter(juce::AudioProcessor& processor, Identifier identifier){
    return addParameterToProcessor(
        processor,
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{identifier.id, identifier.versionHint}, identifier.name,
            juce::NormalisableRange<float>{0.1f, 100.f, 0.1f, 0.5f}, 
            1.f,
            juce::AudioParameterFloatAttributes{}.withLabel("")));
}

juce::AudioParameterFloat& createGainParameter(juce::AudioProcessor& processor, Identifier identifier){
    return addParameterToProcessor(
        processor,
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{identifier.id, identifier.versionHint}, identifier.name,
            juce::NormalisableRange<float>{-48.f, 48.f, 0.1f, 0.5f}, 
            0.f,
            juce::AudioParameterFloatAttributes{}.withLabel("dB")));

}

juce::AudioParameterBool& createBypassedParameter(
    juce::AudioProcessor& processor, Identifier identifier) {
  return addParameterToProcessor(
      processor,
      std::make_unique<juce::AudioParameterBool>(
          juce::ParameterID{identifier.id, identifier.versionHint}, identifier.name, false));
}

juce::AudioParameterChoice& createSlopeParameter(
    juce::AudioProcessor& processor, Identifier identifier) {
  return addParameterToProcessor(
      processor,
      std::make_unique<juce::AudioParameterChoice>(
          juce::ParameterID{identifier.id, identifier.versionHint},
          identifier.name, 
          juce::StringArray{"12dB/oct", "24dB/oct", "36dB/oct", "48dB/oct", "96dB/oct"}, 0));
}

BoostCutParameters createLowShelfParameters(juce::AudioProcessor& processor) {
    auto versionHint = 1;
    Identifier frequencyIdentifier = {"lowShelfFrequency", "Low Shelf Frequency", versionHint};
    Identifier qIdentifier = {"lowShelfQ", "Low Shelf Q-Factor", versionHint};
    Identifier gainIdentifier = {"lowShelfGain", "Low Shelf Gain", versionHint};
    Identifier bypassIdentifier = {"lowShelfBypass", "Low Shelf Bypass", versionHint};
    Identifier slopeIdentifier = {"lowShelfSlope", "Low Shelf Slope", versionHint};

    auto& frequency = createFrequencyParameter(processor, frequencyIdentifier);
    auto& q = createQParameter(processor, qIdentifier);
    auto& gain = createGainParameter(processor, gainIdentifier);
    auto& slope = createSlopeParameter(processor, slopeIdentifier);
    auto& bypassed = createBypassedParameter(processor, bypassIdentifier);

    BoostCutParameters parameters = {{frequency, q, slope, bypassed}, gain };

    return parameters;
}

std::array<std::unique_ptr<BoostCutParameters>, ParametricEq::NUM_PEAKS> createPeakFilterParameters(juce::AudioProcessor& processor) {
    std::array<std::unique_ptr<BoostCutParameters>, ParametricEq::NUM_PEAKS> parameters{};
    auto versionHint = 1;
    for (size_t i = 0; i < ParametricEq::NUM_PEAKS; i++) {
        auto num = juce::String(i + 1);
        auto name = "Peak " + num + " ";
        auto id = "peak" + num;

        Identifier frequencyIdentifier = {id + "Frequency", name + "Frequency", versionHint};
        Identifier qIdentifier = {id + "QFactor", name + "Q-Factor",  versionHint};
        Identifier gainIdentifier = {id + "Gain", name + "Gain", versionHint};
        Identifier bypassIdentifier = {id + "Bypass", name + "Bypass", versionHint};
        Identifier slopeIdentifier = {id + "Slope", name + "Slope", versionHint};

        auto& frequency = createFrequencyParameter(processor, frequencyIdentifier);
        auto& q = createQParameter(processor, qIdentifier);
        auto& gain = createGainParameter(processor, gainIdentifier);
        auto& slope = createSlopeParameter(processor, slopeIdentifier);
        auto& bypassed = createBypassedParameter(processor, bypassIdentifier);

        parameters[i] = std::unique_ptr<BoostCutParameters>(
            new BoostCutParameters{{frequency, q, slope, bypassed }, gain});
    }
    return parameters;
}
} // namespace

Parameters::Parameters(juce::AudioProcessor& processor)
      : peakFilters{createPeakFilterParameters(processor)},
      lowShelfParameters{createLowShelfParameters(processor)} {}
}  // namespace parametric_eq
