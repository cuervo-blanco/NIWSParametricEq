#include "SimpleParametricEq/FrequencyResponseGUI.h"

namespace parametric_eq {
void FrequencyResponseGUI::paint(juce::Graphics& g) {
    const auto& magnitudes = spectrumMagnitudes_;

    const auto minFreq = 20.0f;
    const auto maxFreq = 20000.0f;

    const auto numBins = static_cast<int>(magnitudes.size());
    if (numBins == 0) {
        return;
    }

    auto bounds = getLocalBounds().toFloat();

    const auto internalMinDb = -100.0f;
    const auto internalMaxDb = 60.0f;

    const auto visibleMinDb = -20.0f;
    const auto visibleMaxDb = 60.0f;

    if (previousMagnitudes_.size() != magnitudes.size()) {
        previousMagnitudes_ = magnitudes;
    }

    std::vector<float> blendedMagnitudes(magnitudes.size());
    const auto blend = 0.5f;

    for (size_t i = 0; i < magnitudes.size(); ++i) {
        const auto currentDb  = juce::jlimit(internalMinDb, internalMaxDb, magnitudes[i]);
        const auto previousDb = juce::jlimit(internalMinDb, internalMaxDb, previousMagnitudes_[i]);

        blendedMagnitudes[i] = (1.0f - blend) * currentDb + blend * previousDb;
    }

    previousMagnitudes_ = magnitudes;

    std::vector<float> displayMagnitudes(blendedMagnitudes.size());

    if (blendedMagnitudes.size() >= 3) {
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
    bool pathStarted = false;

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

        auto normX = (std::log10(freq) - std::log10(minFreq)) /
            (std::log10(maxFreq) - std::log10(minFreq));

        auto x = juce::jmap(normX, bounds.getX(), bounds.getRight());

        auto rawDb = displayMagnitudes[static_cast<size_t>(bin)];
        rawDb = juce::jlimit(internalMinDb, internalMaxDb, rawDb);

        float dbForY = juce::jlimit(visibleMinDb, visibleMaxDb, rawDb);

        auto normY = juce::jmap(dbForY, visibleMinDb, visibleMaxDb, 1.0f, 0.0f);
        auto y = juce::jmap(normY, 0.0f, 1.0f, bounds.getY(), bounds.getBottom());

        if (!pathStarted) {
            spectrumPath.startNewSubPath(x, y);
            pathStarted = true;
        } else {
            spectrumPath.lineTo(x, y);
        }
    }

    juce::Graphics::ScopedSaveState saveState(g);

    auto clip = bounds;
    clip.removeFromBottom(1.0f);
    g.reduceClipRegion(clip.toNearestInt());

    g.setColour(juce::Colours::white);
    g.strokePath(spectrumPath, juce::PathStrokeType(1.5f));
}
}// namespace parametric_eq
