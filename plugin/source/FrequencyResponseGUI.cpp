#include "SimpleParametricEq/FrequencyResponseGUI.h"
#include "SimpleParametricEq/gui/FrequencyMapping.h"

namespace parametric_eq {
void FrequencyResponseGUI::paint(juce::Graphics& g) {
    const auto numBins = static_cast<int>(spectrumMagnitudes_.size());
    if (numBins == 0) {
        return;
    }

    auto bounds = getLocalBounds().toFloat();

    const auto visibleHeadroomDb = 40.0f;
    const auto referenceDb = 60.0f;

    if (previousMagnitudes_.empty() || previousMagnitudes_.size() != spectrumMagnitudes_.size()) {
        previousMagnitudes_ = spectrumMagnitudes_;
    }

    std::vector<float> blendedMagnitudes(spectrumMagnitudes_.size());
    const auto blend = 0.5f;

    for (size_t i = 0; i < spectrumMagnitudes_.size(); ++i) {
        const auto currentDb = spectrumMagnitudes_[i];
        const auto previousDb = previousMagnitudes_[i];
        blendedMagnitudes[i] = (1.0f - blend) * currentDb + blend * previousDb;
    }

    previousMagnitudes_ = spectrumMagnitudes_;

    std::vector<float> displayMagnitudes(blendedMagnitudes.size());

    displayMagnitudes = blendedMagnitudes;

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

        const auto calibratedDb = rawDb - referenceDb;

        auto dbForY = juce::jlimit(-visibleHeadroomDb, visibleHeadroomDb, calibratedDb);

        auto normY = juce::jmap(dbForY, -visibleHeadroomDb, visibleHeadroomDb, 1.0f, 0.0f);
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
