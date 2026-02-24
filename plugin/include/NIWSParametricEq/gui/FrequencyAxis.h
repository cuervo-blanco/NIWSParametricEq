#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include <cmath>

#include "../filters/BiquadFilter.h"

namespace parametric_eq {
class FrequencyAxis : public juce::Component {
public:
    FrequencyAxis() = default;
    ~FrequencyAxis() override = default;

    void paint(juce::Graphics& g) override;

    void setBands (const std::vector<BiquadFilter*>& bands) {
        bands_ = bands;
        repaint();
    }

    void setReferenceBands (const std::vector<BiquadFilter*>& bands) {
        referenceBands_ = bands;
        repaint();
    }

    void setDbRange(float minDb, float maxDb) noexcept {
        minDb_ = minDb;
        maxDb_ = maxDb;
        repaint();
    }

    float getCombinedMagnitudeDbAt(double freqHz, const std::vector<BiquadFilter*>& bands);

private:
    std::vector<BiquadFilter*> bands_;
    std::vector<BiquadFilter*> referenceBands_;
    float minDb_ = -60.0f; 
    float maxDb_ = +60.0f;

    void drawGrid(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawZeroLine(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawResponse(juce::Graphics& g, juce::Rectangle<float> bounds,
                      const std::vector<BiquadFilter*>& bands,
                      juce::Colour colour,
                      float thickness,
                      float alpha);
};
}  // namespace parametric_eq