#include "SimpleParametricEq/FrequencyResponseGUI.h"
#include "SimpleParametricEq/gui/FrequencyMapping.h"

namespace parametric_eq {
void FrequencyResponseGUI::paint(juce::Graphics& g) {
    const auto& magnitudes = spectrumMagnitudes_;

    const auto numBins = static_cast<int>(magnitudes.size());
    if (numBins == 0) {
        return;
    }

    auto bounds = getLocalBounds().toFloat();

    const auto internalMinDb = -200.0f;
    const auto internalMaxDb = 200.0f;

    const auto visibleHeadroom = 40.0f;
    const auto visibleMinDb = -visibleHeadroom;
    const auto visibleMaxDb = visibleHeadroom;

    const auto referenceDb = 60.0f;

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

    const auto nyquist = static_cast<float>(sampleRate_ * 0.5);

    for (int bin = 0; bin < numBins; ++bin) {
        auto freq = juce::jmap(static_cast<float>(bin),
                               0.0f,
                               static_cast<float>(numBins - 1),
                               0.0f,
                               nyquist);

        if (freq < freqmap::minFreq || freq > freqmap::maxFreq) {
            continue;
        }

        auto x = freqmap::frequencyToX(freq, bounds);

        auto rawDb = displayMagnitudes[static_cast<size_t>(bin)];
        rawDb = juce::jlimit(internalMinDb, internalMaxDb, rawDb);

        const auto calibratedDb = rawDb - referenceDb;

        auto dbForY = juce::jlimit(visibleMinDb, visibleMaxDb, calibratedDb);

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
