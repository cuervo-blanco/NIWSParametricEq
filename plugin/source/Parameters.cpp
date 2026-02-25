#include "NIWSParametricEq/Parameters.h"
#include "NIWSParametricEq/ParametricEq.h"
#include "NIWSParametricEq/filters/FilterParameters.h"

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

juce::AudioParameterFloat& createFrequencyParameter(juce::AudioProcessor& processor, Identifier identifier, float defaultFreq = 1000.f){
    return addParameterToProcessor(
        processor,
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{identifier.id, identifier.versionHint}, identifier.name,
            juce::NormalisableRange<float>{20.f, 20000.f, 1.f, 0.4f}, 
            defaultFreq,
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

juce::AudioParameterFloat& createShelfSlopeParameter(juce::AudioProcessor& processor, Identifier identifier){
    return addParameterToProcessor(
        processor,
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{identifier.id, identifier.versionHint}, identifier.name,
            juce::NormalisableRange<float>{0.1f, 2.f, 0.1f, 0.5f}, 
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
          juce::ParameterID{identifier.id, identifier.versionHint}, 
          identifier.name, 
          false));
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

    auto& frequency = createFrequencyParameter(processor, frequencyIdentifier, 80.f);
    auto& q = createShelfSlopeParameter(processor, qIdentifier);
    auto& gain = createGainParameter(processor, gainIdentifier);
    auto& slope = createSlopeParameter(processor, slopeIdentifier);
    auto& bypassed = createBypassedParameter(processor, bypassIdentifier);

    BoostCutParameters parameters = {{frequency, q, slope, bypassed}, gain };

    return parameters;
}

BoostCutParameters createHighShelfParameters(juce::AudioProcessor& processor) {
    auto versionHint = 1;
    Identifier frequencyIdentifier = {"highShelfFrequency", "High Shelf Frequency", versionHint};
    Identifier qIdentifier = {"highShelfQ", "High Shelf Q-Factor", versionHint};
    Identifier gainIdentifier = {"highShelfGain", "High Shelf Gain", versionHint};
    Identifier bypassIdentifier = {"highShelfBypass", "High Shelf Bypass", versionHint};
    Identifier slopeIdentifier = {"highShelfSlope", "High Shelf Slope", versionHint};

    auto& frequency = createFrequencyParameter(processor, frequencyIdentifier, 15000.f);
    auto& q = createShelfSlopeParameter(processor, qIdentifier);
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
        auto freq = static_cast<float>(*std::next(ParametricEq::DEFAULT_FREQS.begin(), static_cast<int>(i)));

        Identifier frequencyIdentifier = {id + "Frequency", name + "Frequency", versionHint};
        Identifier qIdentifier = {id + "QFactor", name + "Q-Factor",  versionHint};
        Identifier gainIdentifier = {id + "Gain", name + "Gain", versionHint};
        Identifier bypassIdentifier = {id + "Bypass", name + "Bypass", versionHint};
        Identifier slopeIdentifier = {id + "Slope", name + "Slope", versionHint};

        auto& frequency = createFrequencyParameter(processor, frequencyIdentifier, freq);
        auto& q = createQParameter(processor, qIdentifier);
        auto& gain = createGainParameter(processor, gainIdentifier);
        auto& slope = createSlopeParameter(processor, slopeIdentifier);
        auto& bypassed = createBypassedParameter(processor, bypassIdentifier);

        parameters[i] = std::unique_ptr<BoostCutParameters>(
            new BoostCutParameters{{frequency, q, slope, bypassed }, gain});
    }
    return parameters;
}

BaseParameters createLowPassParameters(juce::AudioProcessor& processor) {
    auto versionHint = 1;
    Identifier frequencyIdentifier = {"lowPassFrequency", "Low Pass Frequency", versionHint};
    Identifier qIdentifier = {"lowPassQ", "Low Pass Q-Factor", versionHint};
    Identifier bypassIdentifier = {"lowPassBypass", "Low Pass Bypass", versionHint};
    Identifier slopeIdentifier = {"lowPassSlope", "Low Pass Slope", versionHint};

    auto& frequency = createFrequencyParameter(processor, frequencyIdentifier, 15000.f);
    auto& q = createQParameter(processor, qIdentifier);
    auto& slope = createSlopeParameter(processor, slopeIdentifier);
    auto& bypassed = createBypassedParameter(processor, bypassIdentifier);

    BaseParameters parameters = {frequency, q, slope, bypassed};

    return parameters;
}

BaseParameters createHighPassParameters(juce::AudioProcessor& processor) {
    auto versionHint = 1;
    Identifier frequencyIdentifier = {"highPassFrequency", "High Pass Frequency", versionHint};
    Identifier qIdentifier = {"highPassQ", "High Pass Q-Factor", versionHint};
    Identifier bypassIdentifier = {"highPassBypass", "High Pass Bypass", versionHint};
    Identifier slopeIdentifier = {"highPassSlope", "High Pass Slope", versionHint};

    auto& frequency = createFrequencyParameter(processor, frequencyIdentifier, 40.f);
    auto& q = createQParameter(processor, qIdentifier);
    auto& slope = createSlopeParameter(processor, slopeIdentifier);
    auto& bypassed = createBypassedParameter(processor, bypassIdentifier);

    BaseParameters parameters = {frequency, q, slope, bypassed};

    return parameters;
}
} // namespace

Parameters::Parameters(juce::AudioProcessor& processor)
      : bypassed{createBypassedParameter(processor, {"bypassed", "Bypass", 1})},
      peakFilters{createPeakFilterParameters(processor)},
      lowShelfParameters{createLowShelfParameters(processor)},  
      highShelfParameters{createHighShelfParameters(processor)},  
      lowPassParameters{createLowPassParameters(processor)},
      highPassParameters{createHighPassParameters(processor)},
      isPost{createBypassedParameter(processor, {"isPost", "Post", 1})} {}
}  // namespace parametric_eq
