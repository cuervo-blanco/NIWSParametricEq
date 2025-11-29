#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

struct LowShelfParameters {
    juce::AudioParameterFloat& lowShelfFrequency;
    juce::AudioParameterFloat& lowShelfQ;
    juce::AudioParameterFloat& lowShelfGain;
};