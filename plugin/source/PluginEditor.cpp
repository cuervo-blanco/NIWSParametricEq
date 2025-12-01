#include "SimpleParametricEq/PluginEditor.h"
#include "SimpleParametricEq/PluginProcessor.h"

namespace parametric_eq {
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(
    AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p) {
  juce::ignoreUnused(processorRef);
  setSize(1080, 540);
  startTimerHz(60);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    if (spectrumMagnitudes_.empty()) {
        return;
    }

    auto bounds = getLocalBounds().toFloat().reduced(10.0f);

    g.setColour(juce::Colours::white);

    const auto minFreq = 20.0f;
    const auto maxFreq = static_cast<float>(processorRef.getSampleRate() * 0.5);

    const auto numBins = static_cast<int>(spectrumMagnitudes_.size());

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

        auto db = spectrumMagnitudes_[static_cast<size_t>(bin)];
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

void AudioPluginAudioProcessorEditor::resized() {}

void AudioPluginAudioProcessorEditor::timerCallback() {
    auto& analyzer = processorRef.getSpectrumAnalyzer();

    if (analyzer.isNewFFTReady()) {
        const auto& mags = analyzer.getMagnitudesDb();

        spectrumMagnitudes_ = mags;

        analyzer.clearNewFFTFlag();

        repaint();
    }
}

}  // namespace parametric_eq
