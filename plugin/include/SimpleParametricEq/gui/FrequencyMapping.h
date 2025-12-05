#pragma once

#include <juce_graphics/juce_graphics.h>
#include <cmath>

namespace parametric_eq::freqmap {

    static constexpr float minFreq = 20.0f;
    static constexpr float maxFreq = 20000.0f;

    inline float frequencyToX (float freq, const juce::Rectangle<float>& bounds) noexcept {
        freq = juce::jlimit (minFreq, maxFreq, freq);

        const auto logMin = std::log10 (minFreq);
        const auto logMax = std::log10 (maxFreq);
        const auto norm = (std::log10 (freq) - logMin) / (logMax - logMin);

        return bounds.getX() + norm * bounds.getWidth();
    }

} // namespace parametric_eq::freqmap
