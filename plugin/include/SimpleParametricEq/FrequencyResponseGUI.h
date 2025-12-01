#pragma once
#include "PluginProcessor.h"
#include <juce_graphics/juce_graphics.h>

namespace parametric_eq {
class FrequencyResponseGUI {
public:
    FrequencyResponseGUI() = default;
    ~FrequencyResponseGUI() = default;

    void paint(juce::Graphics& g, const std::vector<float>& magnitudes, 
                      const AudioPluginAudioProcessor& processorRef);
    void setBounds(const juce::Rectangle<float>& newBounds) noexcept {
        bounds = newBounds;
    }
private:
    juce::Rectangle<float> bounds;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FrequencyResponseGUI)
};
}  // namespace parametric_eq