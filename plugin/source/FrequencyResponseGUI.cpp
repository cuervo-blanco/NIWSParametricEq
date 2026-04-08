#include "NIWSParametricEq/FrequencyResponseGUI.h"
#include "NIWSParametricEq/gui/FrequencyMapping.h"

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

    const auto nyquist = static_cast<float>(sampleRate_ * 0.5);
    const auto baselineY = bounds.getBottom() - 1.0f;
    constexpr auto minStemSpacingPx = 8.0f;

    struct StemPoint {
        float x;
        float y;
    };

    std::vector<StemPoint> stemPoints;
    stemPoints.reserve(static_cast<size_t>(numBins));

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
        if (!stemPoints.empty() && std::abs(x - stemPoints.back().x) < minStemSpacingPx) {
            if (y < stemPoints.back().y) {
                stemPoints.back() = { x, y };
            }
        } else {
            stemPoints.push_back({ x, y });
        }
    }

    juce::Graphics::ScopedSaveState saveState(g);

    auto clip = bounds;
    clip.removeFromBottom(1.0f);
    g.reduceClipRegion(clip.toNearestInt());

    g.setColour(juce::Colours::white);
    constexpr auto stemThickness = 1.2f;
    constexpr auto markerRadius = 2.5f;

    for (const auto& point : stemPoints) {
        g.drawLine(point.x, baselineY, point.x, point.y, stemThickness);
        g.fillEllipse(point.x - markerRadius,
                      point.y - markerRadius,
                      markerRadius * 2.0f,
                      markerRadius * 2.0f);
    }
}
}// namespace parametric_eq
