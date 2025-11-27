#include "SimpleParametricEq/Parameters.h"
#include "SimpleParametricEq/ParametricEq.h"
namespace parametric_eq {
namespace {
auto& addParameterToProcessor(juce::AudioProcessor& processor, auto parameter) {
  auto& result = *parameter;
  processor.addParameter(parameter.release());
  return result;
}

std::array<juce::AudioParameterFloat*, ParametricEq::NUM_BANDS> createPeakFreqsParameters(juce::AudioProcessor& processor) {
    std::array<juce::AudioParameterFloat*, ParametricEq::NUM_BANDS> parameters{};
    constexpr auto versionHint = 1;
    for (size_t i = 0; i < ParametricEq::NUM_BANDS; i++) {
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

std::array<juce::AudioParameterFloat*, ParametricEq::NUM_BANDS> createPeakQsParameters(juce::AudioProcessor& processor) {
    std::array<juce::AudioParameterFloat*, ParametricEq::NUM_BANDS> parameters{};
    constexpr auto versionHint = 1;
    for (size_t i = 0; i < ParametricEq::NUM_BANDS; i++) {
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

std::array<juce::AudioParameterFloat*, ParametricEq::NUM_BANDS> createPeakGainsParameters(juce::AudioProcessor& processor) {
    std::array<juce::AudioParameterFloat*, ParametricEq::NUM_BANDS> parameters{};
    constexpr auto versionHint = 1;
    for (size_t i = 0; i < ParametricEq::NUM_BANDS; i++) {
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

} // namespace
Parameters::Parameters(juce::AudioProcessor& processor)
    : peakFreqs{createPeakFreqsParameters(processor)},
      peakQs{createPeakQsParameters(processor)},
      peakGains{createPeakGainsParameters(processor)} {}
}  // namespace parametric_eq
