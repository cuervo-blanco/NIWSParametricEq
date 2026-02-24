#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "BiquadFilter.h"

struct BaseParameters {
    juce::AudioParameterFloat& frequency; 
    juce::AudioParameterFloat& qFactor;
    juce::AudioParameterChoice& slope; 
    juce::AudioParameterBool& bypassed; 
};

struct BoostCutParameters {
    BaseParameters base;
    juce::AudioParameterFloat& gain;
};