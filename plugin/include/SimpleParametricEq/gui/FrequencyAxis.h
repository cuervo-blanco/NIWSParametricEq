#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include <cmath>

namespace parametric_eq {
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
    float minDb_ = -60.0f; 
    float maxDb_ = 60.0f;
};
}  // namespace parametric_eq