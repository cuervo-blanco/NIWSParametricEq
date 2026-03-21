#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "BiquadFilter.h"

struct BaseParameters {
    juce::AudioParameterFloat& frequency; 
    juce::AudioParameterFloat& qFactor;
    juce::AudioParameterChoice& slope; 
    juce::AudioParameterBool& bypassed; 
};

struct LfoParameters {
    juce::AudioParameterBool& enabled;
    juce::AudioParameterFloat& rateHz;
    juce::AudioParameterFloat& depth;
    juce::AudioParameterChoice& waveform;
    juce::AudioParameterChoice& polarity;
};

struct BoostCutParameters {
    BaseParameters base;
    juce::AudioParameterFloat& gain;
    LfoParameters lfo;
};
