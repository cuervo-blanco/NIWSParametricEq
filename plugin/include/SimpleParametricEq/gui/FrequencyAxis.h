#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include <cmath>

class FrequencyAxis : public juce::Component {
public:
    FrequencyAxis() = default;
    ~FrequencyAxis() override = default;

    void paint(juce::Graphics& g) override;

    void setDbRange(float minDb, float maxDb) noexcept {
        minDb_ = minDb;
        maxDb_ = maxDb;
        repaint();
    }

private:
    static constexpr float minFreq = 20.0f;
    static constexpr float maxFreq = 20000.0f;
    float minDb_ = -60.0f; 
    float maxDb_ = 60.0f;

    static float frequencyToX (float freq, const juce::Rectangle<float>& bounds) noexcept {
        freq = juce::jlimit (minFreq, maxFreq, freq);

        const float logMin = std::log10 (minFreq);
        const float logMax = std::log10 (maxFreq);
        const float norm   = (std::log10 (freq) - logMin) / (logMax - logMin);

        return bounds.getX() + norm * bounds.getWidth();
    }
};