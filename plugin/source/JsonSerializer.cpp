#include <juce_audio_processors/juce_audio_processors.h>
#include "NIWSParametricEq/JsonSerializer.h" 
#include "NIWSParametricEq/Parameters.h" 

namespace parametric_eq {
namespace {

struct SerializableBaseParameters {
  float frequency = 0.0f;
  float qFactor = 0.707f;
  juce::String slope; 
  bool bypassed = false;

  static constexpr int marshallingVersion = 1;

  template <typename Archive, typename T>
  static void serialise(Archive& archive, T& t) {
    using namespace juce;
    archive(named("frequency", t.frequency),
            named("qFactor", t.qFactor),
            named("slope", t.slope),
            named("bypassed", t.bypassed));
  }
};

struct SerializableBoostCutParameters {
  SerializableBaseParameters base;
  float gain = 0.0f;

  static constexpr int marshallingVersion = 1;

  template <typename Archive, typename T>
  static void serialise(Archive& archive, T& t) {
    using namespace juce;
    archive(named("base", t.base),
            named("gain", t.gain));
  }
};

struct SerializableParameters {
  bool bypassed = false;
  bool isPost = false;

  std::array<SerializableBoostCutParameters, ParametricEq::NUM_PEAKS> peakFilters{};

  SerializableBoostCutParameters lowShelf{};
  SerializableBoostCutParameters highShelf{};

  SerializableBaseParameters lowPass{};
  SerializableBaseParameters highPass{};

  static constexpr int marshallingVersion = 1;

  template <typename Archive, typename T>
  static void serialise(Archive& archive, T& t) {
    using namespace juce;

    if (archive.getVersion() != marshallingVersion) {
      return;
    }

    std::string pluginName = JucePlugin_Name;
    archive(named("pluginName", pluginName));

    if (pluginName != JucePlugin_Name)
      return;

    archive(
      named("bypassed", t.bypassed),
      named("isPost", t.isPost),

      named("peakFilters", t.peakFilters),
      named("lowShelf", t.lowShelf),
      named("highShelf", t.highShelf),
      named("lowPass", t.lowPass),
      named("highPass", t.highPass)
    );
  }
};

static SerializableBaseParameters from(const BaseParameters& p) {
  return {
    .frequency = p.frequency.get(),
    .qFactor = p.qFactor.get(),
    .slope = p.slope.getCurrentChoiceName(),
    .bypassed = p.bypassed.get()
  };
}

static SerializableBoostCutParameters from(const BoostCutParameters& p) {
  return {
    .base = from(p.base),
    .gain = p.gain.get()
  };
}

static SerializableParameters from(const parametric_eq::Parameters& parameters) {
  SerializableParameters out{};
  out.bypassed = parameters.bypassed.get();
  out.isPost = parameters.isPost.get();

  for (size_t i = 0; i < ParametricEq::NUM_PEAKS; ++i) {
    out.peakFilters[i] = from(*parameters.peakFilters[i]);
  }

  out.lowShelf = from(parameters.lowShelfParameters);
  out.highShelf = from(parameters.highShelfParameters);

  out.lowPass = from(parameters.lowPassParameters);
  out.highPass = from(parameters.highPassParameters);

  return out;
}

static int choiceNameToIndex(const juce::StringArray& choices, const juce::String& name, int fallbackIndex = 0) {
  const auto idx = choices.indexOf(name);
  return idx >= 0 ? idx : fallbackIndex;
}

static void apply(BaseParameters& dst, const SerializableBaseParameters& src) {
  dst.frequency = src.frequency;
  dst.qFactor = src.qFactor;
  dst.bypassed = src.bypassed;

  const auto slopeIndex = choiceNameToIndex(dst.slope.choices, src.slope, dst.slope.getIndex());
  dst.slope = slopeIndex;
}

static void apply(BoostCutParameters& dst, const SerializableBoostCutParameters& src) {
  apply(dst.base, src.base);
  dst.gain = src.gain;
}

} // namespace

void JsonSerializer::serialize(const Parameters& parameters, juce::OutputStream& output) {
  const auto parametersToSerialize = from(parameters);
  const auto json = juce::ToVar::convert(parametersToSerialize);

  if (!json.has_value()) {
    return;
  }

  juce::JSON::writeToStream(
    output,
    *json,
    juce::JSON::FormatOptions{}
      .withSpacing(juce::JSON::Spacing::multiLine)
      .withMaxDecimalPlaces(2)
  );
}

juce::Result JsonSerializer::deserialize(juce::InputStream& input, Parameters& parameters) {
  juce::var parsedResult;
  const auto result = juce::JSON::parse(input.readEntireStreamAsString(), parsedResult);
  if (result.failed()) {
    return result;
  }

  const auto parsed = juce::FromVar::convert<SerializableParameters>(parsedResult);
  if (!parsed.has_value()) {
    return juce::Result::fail("failed to parse parameters from JSON representation");
  }

  parameters.bypassed = parsed->bypassed;
  parameters.isPost = parsed->isPost;

  for (size_t i = 0; i < ParametricEq::NUM_PEAKS; ++i) {
    apply(*parameters.peakFilters[i], parsed->peakFilters[i]);
  }

  apply(parameters.lowShelfParameters, parsed->lowShelf);
  apply(parameters.highShelfParameters, parsed->highShelf);

  apply(parameters.lowPassParameters,  parsed->lowPass);
  apply(parameters.highPassParameters, parsed->highPass);

  return juce::Result::ok();
}

} // namespace parametric_eq