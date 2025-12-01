#include "SimpleParametricEq/FrequencyResponseGUI.h"

namespace parametric_eq {
void FrequencyResponseGUI::paint(juce::Graphics& g, const std::vector<float>& magnitudes, 
                    const AudioPluginAudioProcessor& processorRef) {
    const auto minFreq = 20.0f;
    const auto maxFreq = static_cast<float>(processorRef.getSampleRate() * 0.5);

    const auto numBins = static_cast<int>(magnitudes.size());

    const auto minDb = 0.0f;
    const auto maxDb = 60.0f;

    juce::Path spectrumPath;

    for (int bin = 0; bin < numBins; ++bin) {
        auto freq = juce::jmap(static_cast<float>(bin), 0.0f, static_cast<float>(numBins - 1), 0.0f, maxFreq);

        freq = std::max(freq, minFreq);

        auto normX =
            (std::log10(freq) - std::log10(minFreq)) /
            (std::log10(maxFreq) - std::log10(minFreq));

        auto x = juce::jmap(normX, bounds.getX(), bounds.getRight());

        auto db = magnitudes[static_cast<size_t>(bin)];
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