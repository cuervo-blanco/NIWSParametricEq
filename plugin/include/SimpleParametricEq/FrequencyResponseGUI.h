#pragma once
#include "PluginProcessor.h"
#include <juce_graphics/juce_graphics.h>

namespace parametric_eq {
class FrequencyResponseGUI : public juce::Component {
public:
    FrequencyResponseGUI() = default;
    ~FrequencyResponseGUI() override = default;

    void paint(juce::Graphics& g) override;
    void setMagnitudes(const std::vector<float>& magnitudes) {
        spectrumMagnitudes_ = magnitudes;
        repaint();
    }

private:
    std::vector<float> spectrumMagnitudes_; 
    std::vector<float> previousMagnitudes_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FrequencyResponseGUI)
};
}  // namespace parametric_eq