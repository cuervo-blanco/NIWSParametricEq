#include "NIWSParametricEq/FilterInspectorPanel.h"

#include <array>
#include <cmath>
#include <vector>

namespace parametric_eq {
namespace {
const auto panelBackground = juce::Colour(13, 13, 13).withAlpha(0.94f);
const auto panelOutline = juce::Colour(222, 140, 0).withAlpha(0.85f);
const auto panelText = juce::Colour(242, 242, 242);
const auto panelMutedText = juce::Colour(170, 170, 170);
const auto panelControlBackground = juce::Colour(28, 28, 28);
const auto panelControlOutline = juce::Colour(86, 58, 18);

void styleLabel(juce::Label& label, juce::Justification justification, float fontHeight) {
    label.setJustificationType(justification);
    label.setFont(juce::Font{juce::FontOptions(fontHeight, juce::Font::plain)});
    label.setColour(juce::Label::textColourId, panelText);
}

void styleComboBox(juce::ComboBox& comboBox) {
    comboBox.setColour(juce::ComboBox::backgroundColourId, panelControlBackground);
    comboBox.setColour(juce::ComboBox::outlineColourId, panelControlOutline);
    comboBox.setColour(juce::ComboBox::textColourId, panelText);
    comboBox.setColour(juce::ComboBox::arrowColourId, panelText);
}

void styleSlider(juce::Slider& slider) {
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 84, 20);
    slider.setColour(juce::Slider::backgroundColourId, panelControlBackground);
    slider.setColour(juce::Slider::thumbColourId, panelOutline);
    slider.setColour(juce::Slider::trackColourId, panelOutline);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, panelControlBackground);
    slider.setColour(juce::Slider::textBoxOutlineColourId, panelControlOutline);
    slider.setColour(juce::Slider::textBoxTextColourId, panelText);
}

void styleToggleButton(juce::ToggleButton& button) {
    button.setButtonText({});
    button.setColour(juce::ToggleButton::tickColourId, panelOutline);
    button.setColour(juce::ToggleButton::tickDisabledColourId, panelMutedText);
    button.setColour(juce::ToggleButton::textColourId, panelText);
}

int getDecimalPlaces(float interval) {
    if (interval <= 0.0f) {
        return 2;
    }

    if (interval >= 1.0f) {
        return 0;
    }

    if (interval >= 0.1f) {
        return 1;
    }

    if (interval >= 0.01f) {
        return 2;
    }

    return 3;
}
}  // namespace

FilterInspectorPanel::SliderField::SliderField(juce::String labelText) {
    addAndMakeVisible(label);
    addAndMakeVisible(slider);

    label.setText(std::move(labelText), juce::dontSendNotification);
    styleLabel(label, juce::Justification::centredLeft, 14.0f);
    styleSlider(slider);
}

void FilterInspectorPanel::SliderField::bind(juce::AudioParameterFloat& parameter) {
    attachment.reset();

    juce::NormalisableRange<double> sliderRange {
        static_cast<double>(parameter.range.start),
        static_cast<double>(parameter.range.end),
        static_cast<double>(parameter.range.interval),
        static_cast<double>(parameter.range.skew),
        parameter.range.symmetricSkew
    };
    slider.setNormalisableRange(sliderRange);
    slider.setNumDecimalPlacesToDisplay(getDecimalPlaces(parameter.range.interval));

    const auto suffix = parameter.getLabel();
    slider.setTextValueSuffix(suffix.isNotEmpty() ? " " + suffix : juce::String{});

    attachment = std::make_unique<juce::SliderParameterAttachment>(parameter, slider);
    setVisible(true);
}

void FilterInspectorPanel::SliderField::unbind() {
    attachment.reset();
    setVisible(false);
}

void FilterInspectorPanel::SliderField::resized() {
    auto bounds = getLocalBounds();
    label.setBounds(bounds.removeFromLeft(100));
    slider.setBounds(bounds);
}

FilterInspectorPanel::ChoiceField::ChoiceField(juce::String labelText) {
    addAndMakeVisible(label);
    addAndMakeVisible(comboBox);

    label.setText(std::move(labelText), juce::dontSendNotification);
    styleLabel(label, juce::Justification::centredLeft, 14.0f);
    styleComboBox(comboBox);
}

void FilterInspectorPanel::ChoiceField::bind(juce::AudioParameterChoice& parameter) {
    attachment.reset();
    comboBox.clear(juce::dontSendNotification);

    for (int i = 0; i < parameter.choices.size(); ++i) {
        comboBox.addItem(parameter.choices[i], i + 1);
    }

    attachment = std::make_unique<juce::ComboBoxParameterAttachment>(parameter, comboBox);
    setVisible(true);
}

void FilterInspectorPanel::ChoiceField::unbind() {
    attachment.reset();
    comboBox.clear(juce::dontSendNotification);
    setVisible(false);
}

void FilterInspectorPanel::ChoiceField::resized() {
    auto bounds = getLocalBounds();
    label.setBounds(bounds.removeFromLeft(100));
    comboBox.setBounds(bounds);
}

FilterInspectorPanel::ToggleField::ToggleField(juce::String labelText) {
    addAndMakeVisible(label);
    addAndMakeVisible(button);

    label.setText(std::move(labelText), juce::dontSendNotification);
    styleLabel(label, juce::Justification::centredLeft, 14.0f);
    styleToggleButton(button);
}

void FilterInspectorPanel::ToggleField::bind(juce::AudioParameterBool& parameter) {
    attachment.reset();
    attachment = std::make_unique<juce::ButtonParameterAttachment>(parameter, button);
    setVisible(true);
}

void FilterInspectorPanel::ToggleField::unbind() {
    attachment.reset();
    setVisible(false);
}

void FilterInspectorPanel::ToggleField::resized() {
    auto bounds = getLocalBounds();
    label.setBounds(bounds.removeFromLeft(100));
    button.setBounds(bounds.removeFromLeft(28));
}

FilterInspectorPanel::FilterInspectorPanel() {
    addAndMakeVisible(titleLabel_);
    addAndMakeVisible(hintLabel_);

    styleLabel(titleLabel_, juce::Justification::centredLeft, 18.0f);
    titleLabel_.setFont(juce::Font{juce::FontOptions(18.0f, juce::Font::bold)});

    hintLabel_.setJustificationType(juce::Justification::centredLeft);
    hintLabel_.setFont(juce::Font{juce::FontOptions(13.0f, juce::Font::plain)});
    hintLabel_.setColour(juce::Label::textColourId, panelMutedText);

    const auto addField = [this](auto& field) {
        addAndMakeVisible(field);
        field.setVisible(false);
    };

    addField(frequencyField_);
    addField(qField_);
    addField(slopeField_);
    addField(bypassField_);
    addField(gainField_);
    addField(lfoEnabledField_);
    addField(lfoRateField_);
    addField(lfoDepthField_);
    addField(lfoWaveformField_);
    addField(lfoPolarityField_);

    setVisible(false);
}

void FilterInspectorPanel::showSelection(Selection selection) {
    selection_ = selection;
    refreshFields();
    setVisible(hasSelection());
    resized();
    repaint();
}

void FilterInspectorPanel::clearSelection() {
    selection_ = {};
    refreshFields();
    setVisible(false);
}

bool FilterInspectorPanel::hasSelection() const noexcept {
    return selection_.base != nullptr;
}

void FilterInspectorPanel::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    g.setColour(panelBackground);
    g.fillRoundedRectangle(bounds, 14.0f);

    g.setColour(panelOutline);
    g.drawRoundedRectangle(bounds.reduced(0.5f), 14.0f, 1.0f);

    g.setColour(panelOutline.withAlpha(0.22f));
    g.drawLine(bounds.getX() + 12.0f, bounds.getY() + 31.0f,
               bounds.getRight() - 12.0f, bounds.getY() + 31.0f, 1.0f);
}

void FilterInspectorPanel::resized() {
    auto bounds = getLocalBounds().reduced(10);
    auto headerBounds = bounds.removeFromTop(22);

    titleLabel_.setBounds(headerBounds);
    hintLabel_.setBounds(bounds.removeFromTop(16));
    bounds.removeFromTop(6);

    std::array<juce::Component*, 10> fields {
        &frequencyField_,
        &qField_,
        &slopeField_,
        &bypassField_,
        &gainField_,
        &lfoEnabledField_,
        &lfoRateField_,
        &lfoDepthField_,
        &lfoWaveformField_,
        &lfoPolarityField_,
    };

    std::vector<juce::Component*> visibleFields;
    visibleFields.reserve(fields.size());

    for (auto* field : fields) {
        if (field->isVisible()) {
            visibleFields.push_back(field);
        }
    }

    if (visibleFields.empty()) {
        return;
    }

    const auto columns = visibleFields.size() > 6 ? 3 : (visibleFields.size() > 2 ? 2 : 1);
    const auto rowsPerColumn = static_cast<int>(std::ceil(
        static_cast<float>(visibleFields.size()) / static_cast<float>(columns)));
    const auto columnGap = 12;
    const auto rowGap = 6;

    auto contentBounds = bounds;
    const auto columnWidth = (contentBounds.getWidth() - (columnGap * (columns - 1))) / columns;
    const auto rowHeight = (contentBounds.getHeight() - (rowGap * juce::jmax(0, rowsPerColumn - 1)))
        / juce::jmax(1, rowsPerColumn);

    for (int index = 0; index < static_cast<int>(visibleFields.size()); ++index) {
        const auto columnIndex = index / rowsPerColumn;
        const auto rowIndex = index % rowsPerColumn;
        const auto x = contentBounds.getX() + columnIndex * (columnWidth + columnGap);
        const auto y = contentBounds.getY() + rowIndex * (rowHeight + rowGap);

        visibleFields[static_cast<size_t>(index)]->setBounds(x, y, columnWidth, rowHeight);
    }
}

void FilterInspectorPanel::refreshFields() {
    auto hideAll = [this]() {
        frequencyField_.unbind();
        qField_.unbind();
        slopeField_.unbind();
        bypassField_.unbind();
        gainField_.unbind();
        lfoEnabledField_.unbind();
        lfoRateField_.unbind();
        lfoDepthField_.unbind();
        lfoWaveformField_.unbind();
        lfoPolarityField_.unbind();
    };

    hideAll();

    if (!hasSelection()) {
        titleLabel_.setText({}, juce::dontSendNotification);
        hintLabel_.setText({}, juce::dontSendNotification);
        return;
    }

    titleLabel_.setText(selection_.title, juce::dontSendNotification);
    hintLabel_.setText("This panel shows the full settings for the selected filter, including LFO modulation when available.",
                       juce::dontSendNotification);

    frequencyField_.bind(selection_.base->frequency);
    qField_.bind(selection_.base->qFactor);
    slopeField_.bind(selection_.base->slope);
    bypassField_.bind(selection_.base->bypassed);

    if (selection_.gain != nullptr) {
        gainField_.bind(*selection_.gain);
    }

    if (selection_.lfo != nullptr) {
        lfoEnabledField_.bind(selection_.lfo->enabled);
        lfoRateField_.bind(selection_.lfo->rateHz);
        lfoDepthField_.bind(selection_.lfo->depth);
        lfoWaveformField_.bind(selection_.lfo->waveform);
        lfoPolarityField_.bind(selection_.lfo->polarity);
    }
}
}  // namespace parametric_eq
