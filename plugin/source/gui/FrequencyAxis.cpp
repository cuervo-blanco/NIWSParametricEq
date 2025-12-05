#include "SimpleParametricEq/gui/FrequencyAxis.h"

void FrequencyAxis::paint (juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    juce::FontOptions fontOptions(12.0f);

    g.setColour (juce::Colours::transparentBlack);
    g.fillAll();

    g.setColour (juce::Colours::white);
    g.setFont (fontOptions);

    static constexpr std::array<float, 13> freqTicks {
        20.0f, 30.0f, 40.0f, 50.0f,
        100.0f, 200.0f, 500.0f,
        1000.0f, 2000.0f, 5000.0f,
        10000.0f, 15000.0f, 20000.0f
    };

    const float top = bounds.getY();
    const float bottom = bounds.getBottom();

    for (auto freq : freqTicks) {
        const float x = frequencyToX(freq, bounds);

        const bool isDecade = (std::fmod(std::log10(freq), 1.0f) == 0.0f);
        const float thickness = isDecade ? 1.5f : 0.7f;

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

    const float zeroNorm = juce::jmap(0.0f, minDb_, maxDb_, 1.0f, 0.0f);
    const float zeroY = juce::jmap(zeroNorm, 0.0f, 1.0f,
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