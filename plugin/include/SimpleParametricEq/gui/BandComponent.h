#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "../filters/BiquadFilter.h" 
#include "FrequencyAxis.h"
#include "FrequencyMapping.h"

class BandComponent : public juce::Component {
public:
    enum class BandType {
        LowPass,
        HighPass,
        Peak,
        LowShelf,
        HighShelf,
        BandPass,
        Notch,
        Allpass
    };

    BandComponent(juce::AudioParameterFloat& freqParam,
                  juce::AudioParameterFloat& gainParam,
                  parametric_eq::FrequencyAxis& axis,
                  BandType type)
        : freqParam_(freqParam),
          gainParam_(gainParam),
          axis_(axis),
          type_(type)
    {
        setInterceptsMouseClicks(true, false);

        freq_ = static_cast<double>(freqParam_);
        gainDb_ = gainParam_;
    }

    void setDbRange (float minDb, float maxDb) {
        minDb_ = minDb;
        maxDb_ = maxDb;
        repaint();
    }

    BandType getType() const noexcept { return type_; }

    void updateFromParameters() {
        freq_ = static_cast<double>(freqParam_);
        gainDb_ = gainParam_;
        repaint();
    }

    void mouseDown (const juce::MouseEvent& e) override {
        dragStartPos_ = e.getPosition().toFloat();
        startFreq_ = freq_;
        startGainDb_ = gainDb_;

        freqParam_.beginChangeGesture();
        gainParam_.beginChangeGesture();
    }

    void mouseDrag (const juce::MouseEvent& e) override {
        auto bounds = getLocalBounds().toFloat();
        auto pos = e.getPosition().toFloat();

        const auto dx = pos.x - dragStartPos_.x;
        const auto dy = pos.y - dragStartPos_.y;

        const auto minFreq = freqParam_.range.start;
        const auto maxFreq = freqParam_.range.end;
        const auto logMin = static_cast<double>(std::log10(minFreq));
        const auto logMax = static_cast<double>(std::log10(maxFreq));
        const auto logRange = logMax - logMin;

        const auto logStart = std::log10(startFreq_);
        const auto dragFracX = (bounds.getWidth() != 0.0f)
                                     ? static_cast<double>(dx / bounds.getWidth())
                                     : 0.0;
        const auto logNew = juce::jlimit(logMin, logMax,
                                         logStart + dragFracX * logRange);
        const auto newFreq = std::pow(10.0, logNew);

        const auto gainRange = maxDb_ - minDb_;
        const auto gainPerPix = gainRange / bounds.getHeight();
        const auto newGainDb = juce::jlimit(minDb_, maxDb_,
                                            startGainDb_ - dy * gainPerPix);

        setFrequencyFromUI(newFreq);
        setGainFromUI(newGainDb);
    }

    void mouseUp (const juce::MouseEvent&) override {
        freqParam_.endChangeGesture();
        gainParam_.endChangeGesture();
    }

    void paint (juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();
        auto centre = getHandlePosition(bounds);
        const auto radius = 6.0f;

        g.setColour(juce::Colours::white);
        g.fillEllipse(centre.x - (radius + 1.0f),
                      centre.y - (radius + 1.0f),
                      2.0f * (radius + 1.0f),
                      2.0f * (radius + 1.0f));

        g.setColour(juce::Colour(222,140,0));
        g.fillEllipse(centre.x - radius,
                      centre.y - radius,
                      2.0f * radius,
                      2.0f * radius);
    }

private:
    void setFrequencyFromUI (double freq) {
        freq_ = juce::jlimit(static_cast<double>(freqParam_.range.start),
                             static_cast<double>(freqParam_.range.end),
                             freq);

        const auto normalised = freqParam_.range.convertTo0to1(static_cast<float>(freq_));
        freqParam_.setValueNotifyingHost(normalised);

        axis_.repaint();
        repaint();
    }

    void setGainFromUI(float gainDb) {
        gainDb_ = juce::jlimit(minDb_, maxDb_, gainDb);

        const auto& range = gainParam_.range; 
        const auto normalised = range.convertTo0to1(gainDb_);
        gainParam_.setValueNotifyingHost(normalised);

        axis_.repaint();
        repaint();
    }

    juce::Point<float> getHandlePosition (juce::Rectangle<float> bounds) const {
        const auto x = parametric_eq::freqmap::frequencyToX(static_cast<float>(freq_), bounds);

        const auto dbClamped = juce::jlimit(minDb_, maxDb_, gainDb_);
        const auto yNorm = juce::jmap(dbClamped, minDb_, maxDb_, 1.0f, 0.0f);
        const auto y = juce::jmap(yNorm, 0.0f, 1.0f,
                                  bounds.getY(), bounds.getBottom());

        return {x, y};
    }

    juce::AudioParameterFloat& freqParam_;
    juce::AudioParameterFloat& gainParam_;
    parametric_eq::FrequencyAxis& axis_;
    BandType type_;

    double freq_{1000.0};
    float gainDb_{0.0f};

    float minDb_{-24.0f};
    float maxDb_{+24.0f};

    juce::Point<float> dragStartPos_;
    double startFreq_{1000.0};
    float startGainDb_{0.0f};
};
