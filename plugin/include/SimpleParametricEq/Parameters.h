#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <memory>

#include "ParametricEq.h"
namespace parametric_eq {
struct Parameters {
    explicit Parameters(juce::AudioProcessor&);

    std::array<juce::AudioParameterFloat*, ParametricEq::NUM_PEAKS> peakFreqs{};
    std::array<juce::AudioParameterFloat*, ParametricEq::NUM_PEAKS> peakQs{};
    std::array<juce::AudioParameterFloat*, ParametricEq::NUM_PEAKS> peakGains{};

    JUCE_DECLARE_NON_COPYABLE(Parameters)
    JUCE_DECLARE_NON_MOVEABLE(Parameters)
};
} // namespace parametric_eq