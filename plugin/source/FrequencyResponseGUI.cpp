#include "SimpleParametricEq/FrequencyResponseGUI.h"

namespace parametric_eq {
void FrequencyResponseGUI::paint(juce::Graphics& g, const std::vector<float>& magnitudes) {
    const auto minFreq = 20.0f;
    const auto maxFreq = 20000.0f;

    const auto numBins = static_cast<int>(magnitudes.size());

    const auto minDb = 0.0f;
    const auto maxDb = 60.0f;
    if (previousMagnitudes_.size() != magnitudes.size())
        previousMagnitudes_ = magnitudes;

    std::vector<float> blendedMagnitudes(magnitudes.size());

    const float blend = 0.5f;
    for (size_t i = 0; i < magnitudes.size(); ++i) {
        blendedMagnitudes[i] =
            (1.0f - blend) * magnitudes[i]
            + blend * previousMagnitudes_[i];
    }

    previousMagnitudes_ = magnitudes;

    std::vector<float> displayMagnitudes(blendedMagnitudes.size());

    if (blendedMagnitudes.size() >= 3) {
        // simple 3-point moving average
        displayMagnitudes[0] =
            0.5f * (blendedMagnitudes[0] + blendedMagnitudes[1]);

        for (size_t i = 1; i < blendedMagnitudes.size() - 1; ++i) {
            displayMagnitudes[i] =
                (blendedMagnitudes[i - 1]
               + blendedMagnitudes[i]
               + blendedMagnitudes[i + 1]) / 3.0f;
        }

        const auto last = blendedMagnitudes.size() - 1;
        displayMagnitudes[last] =
            0.5f * (blendedMagnitudes[last - 1] + blendedMagnitudes[last]);
    } else {
        displayMagnitudes = blendedMagnitudes;
    }

    juce::Path spectrumPath;

    for (int bin = 0; bin < numBins; ++bin) {
        auto freq = juce::jmap(static_cast<float>(bin), 
                               0.0f, 
                               static_cast<float>(numBins - 1), 
                               0.0f, 
                               maxFreq);

        freq = std::max(freq, minFreq);

        if (freq > maxFreq) {
            continue;
        }

        auto normX =
            (std::log10(freq) - std::log10(minFreq)) /
            (std::log10(maxFreq) - std::log10(minFreq));

        auto x = juce::jmap(normX, bounds.getX(), bounds.getRight());

        auto db = displayMagnitudes[static_cast<size_t>(bin)];
        db = juce::jlimit(minDb, maxDb, db);

        auto normY = juce::jmap(db, minDb, maxDb, 1.0f, 0.0f);
        auto y = juce::jmap(normY, 0.0f, 1.0f, bounds.getY(), bounds.getBottom());

        if (bin == 0) {
            spectrumPath.startNewSubPath(x, y);
        } else {
            spectrumPath.lineTo(x, y);
        }
    }
    g.strokePath(spectrumPath, juce::PathStrokeType(1.5f));
}
} // namespace parametric_eq