#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "filters/FilterParameters.h"

namespace parametric_eq {
class FilterInspectorPanel : public juce::Component {
public:
    struct Selection {
        juce::String title;
        BaseParameters* base = nullptr;
        juce::AudioParameterFloat* gain = nullptr;
        LfoParameters* lfo = nullptr;
    };

    FilterInspectorPanel();
    ~FilterInspectorPanel() override = default;

    void showSelection(Selection selection);
    void clearSelection();
    bool hasSelection() const noexcept;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    struct SliderField : juce::Component {
        explicit SliderField(juce::String labelText);

        void bind(juce::AudioParameterFloat& parameter);
        void unbind();
        void resized() override;

        juce::Label label;
        juce::Slider slider;
        std::unique_ptr<juce::SliderParameterAttachment> attachment;
    };

    struct ChoiceField : juce::Component {
        explicit ChoiceField(juce::String labelText);

        void bind(juce::AudioParameterChoice& parameter);
        void unbind();
        void resized() override;

        juce::Label label;
        juce::ComboBox comboBox;
        std::unique_ptr<juce::ComboBoxParameterAttachment> attachment;
    };

    struct ToggleField : juce::Component {
        explicit ToggleField(juce::String labelText);

        void bind(juce::AudioParameterBool& parameter);
        void unbind();
        void resized() override;

        juce::Label label;
        juce::ToggleButton button;
        std::unique_ptr<juce::ButtonParameterAttachment> attachment;
    };

    void refreshFields();

    Selection selection_;

    juce::Label titleLabel_;
    juce::Label hintLabel_;

    SliderField frequencyField_{"Frequency"};
    SliderField qField_{"Q"};
    ChoiceField slopeField_{"Slope"};
    ToggleField bypassField_{"Bypass"};

    SliderField gainField_{"Gain"};
    ToggleField lfoEnabledField_{"LFO Enabled"};
    SliderField lfoRateField_{"LFO Rate"};
    SliderField lfoDepthField_{"LFO Depth"};
    ChoiceField lfoWaveformField_{"LFO Waveform"};
    ChoiceField lfoPolarityField_{"LFO Polarity"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterInspectorPanel)
};
}  // namespace parametric_eq
