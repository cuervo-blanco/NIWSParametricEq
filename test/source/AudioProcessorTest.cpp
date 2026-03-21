#include <NIWSParametricEq/PluginProcessor.h>
#include <gtest/gtest.h>

namespace parametric_eq_test {
TEST(AudioProcessor, Foo) {
  parametric_eq::AudioPluginAudioProcessor processor{};
}

TEST(AudioProcessor, SerializesGainLfoParameters) {
  parametric_eq::AudioPluginAudioProcessor source{};
  auto& sourcePeak = *source.getParameters().peakFilters[0];
  sourcePeak.gain = 3.0f;
  sourcePeak.lfo.enabled = true;
  sourcePeak.lfo.rateHz = 2.5f;
  sourcePeak.lfo.depth = 0.75f;
  sourcePeak.lfo.waveform = 2;
  sourcePeak.lfo.polarity = 1;

  juce::MemoryBlock state;
  source.getStateInformation(state);

  parametric_eq::AudioPluginAudioProcessor restored{};
  restored.setStateInformation(state.getData(), static_cast<int>(state.getSize()));

  const auto& restoredPeak = *restored.getParameters().peakFilters[0];
  EXPECT_NEAR(restoredPeak.gain.get(), 3.0f, 0.01f);
  EXPECT_TRUE(restoredPeak.lfo.enabled.get());
  EXPECT_NEAR(restoredPeak.lfo.rateHz.get(), 2.5f, 0.01f);
  EXPECT_NEAR(restoredPeak.lfo.depth.get(), 0.75f, 0.01f);
  EXPECT_EQ(restoredPeak.lfo.waveform.getIndex(), 2);
  EXPECT_EQ(restoredPeak.lfo.polarity.getIndex(), 1);
}
}  // namespace parametric_eq_test
