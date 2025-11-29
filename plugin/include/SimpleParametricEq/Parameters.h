#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <memory>

#include "ParametricEq.h"
#include "filters/FilterParameters.h"
namespace parametric_eq {
struct Parameters {
    explicit Parameters(juce::AudioProcessor&);

    juce::AudioParameterBool& bypassed; 

    std::array<std::unique_ptr<BoostCutParameters>, ParametricEq::NUM_PEAKS> peakFilters;
    BoostCutParameters lowShelfParameters; 
    BaseParameters lowPassParameters;
    BaseParameters highPassParameters;

    JUCE_DECLARE_NON_COPYABLE(Parameters)
    JUCE_DECLARE_NON_MOVEABLE(Parameters)
};
} // namespace parametric_eq