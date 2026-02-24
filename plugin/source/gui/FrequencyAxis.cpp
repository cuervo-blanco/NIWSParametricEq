#include "NIWSParametricEq/gui/FrequencyAxis.h"
#include "NIWSParametricEq/gui/FrequencyMapping.h"
#include "NIWSParametricEq/filters/BiquadFilter.h"

namespace parametric_eq {
void FrequencyAxis::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    g.fillAll(juce::Colours::transparentBlack);

    drawGrid(g, bounds);
    drawZeroLine(g, bounds);

    if (!referenceBands_.empty()) {
        drawResponse(g, bounds, referenceBands_,
                     juce::Colours::darkgrey, 1.5f, 0.7f);
    }

    if (!bands_.empty()) {
        drawResponse(g, bounds, bands_,
                     juce::Colours::orange, 2.0f, 1.0f);
    }
}

void FrequencyAxis::drawGrid(juce::Graphics& g, juce::Rectangle<float> bounds) {
    juce::FontOptions fontOptions(12.0f);
    g.setFont(fontOptions);

    static constexpr std::array<float, 13> freqTicks {
        20.0f, 30.0f, 40.0f, 50.0f,
        100.0f, 200.0f, 500.0f,
        1000.0f, 2000.0f, 5000.0f,
        10000.0f, 15000.0f, 20000.0f
    };

    const auto top = bounds.getY();
    const auto bottom = bounds.getBottom();

    for (auto freq : freqTicks) {
        const auto x = freqmap::frequencyToX(freq, bounds);

        const auto isDecade = (std::fmod(std::log10(freq), 1.0f) == 0.0f);
        const auto thickness = isDecade ? 1.5f : 0.7f;

        g.setColour(juce::Colours::darkgrey.withAlpha(0.7f));
        g.drawLine(x, top, x, bottom, thickness);

        if (freq >= 100.0f) {
            juce::String label = (freq >= 1000.0f)
                                   ? juce::String(freq / 1000.0f, 1) + "k"
                                   : juce::String(static_cast<int>(freq));

            g.setColour(juce::Colours::white.withAlpha(0.9f));

            g.drawFittedText(label,
                             static_cast<int>(x - 20), static_cast<int>(bottom - 16),
                             40, 14,
                             juce::Justification::centred,
                             1);
        }
    }
}

void FrequencyAxis::drawZeroLine(juce::Graphics& g, juce::Rectangle<float> bounds) {
    const auto zeroNorm = juce::jmap(0.0f, minDb_, maxDb_, 1.0f, 0.0f);
    const auto zeroY = juce::jmap(zeroNorm, 0.0f, 1.0f,
                                      bounds.getY(), bounds.getBottom());

    g.setColour(juce::Colours::orange.withAlpha(0.9f));
    g.drawLine(bounds.getX(), zeroY, bounds.getRight(), zeroY, 1.5f);

    g.drawFittedText("0 dB",
                     static_cast<int>(bounds.getX()) + 4,
                     static_cast<int>(zeroY - 7),
                     40, 14,
                     juce::Justification::left,
                     1);
}

void FrequencyAxis::drawResponse(juce::Graphics& g,
                                 juce::Rectangle<float> bounds,
                                 const std::vector<BiquadFilter*>& bands,
                                 juce::Colour colour,
                                 float thickness,
                                 float alpha) {
    if (bands.empty()) {
        return;
    }

    static constexpr auto numPoints = 512;

    juce::Path path;
    bool started = false;

    const auto minFreq = 20.0;
    const auto maxFreq = 20000.0;
    const double logMin  = std::log10(minFreq);
    const double logMax  = std::log10(maxFreq);

    for (int i = 0; i < numPoints; ++i) {
        const auto t = static_cast<double>(i) / (numPoints - 1);
        const auto logF = juce::jmap(t,  logMin, logMax);
        const double freq = std::pow(10.0, logF);

        const auto magDb = getCombinedMagnitudeDbAt(freq, bands);

        const auto clampedDb = juce::jlimit(minDb_, maxDb_, magDb);

        const auto x = freqmap::frequencyToX(static_cast<float>(freq), bounds);

        const auto yNorm = juce::jmap(clampedDb, minDb_, maxDb_, 1.0f, 0.0f);
        const auto y = juce::jmap(yNorm, 0.0f, 1.0f,
                                  bounds.getY(), bounds.getBottom());

        if (!started) {
            path.startNewSubPath(x, y);
            started = true;
        } else {
            path.lineTo(x, y);
        }
    }

    g.setColour(colour.withAlpha(alpha));
    g.strokePath(path, juce::PathStrokeType(thickness));
}


float FrequencyAxis::getCombinedMagnitudeDbAt(double freqHz,
                                       const std::vector<BiquadFilter*>& bands) {
    auto totalGain = 1.0;

    for (auto* b : bands) {
        if (b != nullptr) {
            totalGain *= static_cast<double>(b->getMagnitudeAtFrequency(freqHz));
        }
    }

    return juce::Decibels::gainToDecibels(static_cast<float>(totalGain));
}

}  // namespace parametric_eq
