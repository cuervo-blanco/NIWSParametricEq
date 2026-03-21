#include "NIWSParametricEq/PluginProcessor.h"
#include "NIWSParametricEq/PluginEditor.h"
#include "NIWSParametricEq/JsonSerializer.h"
#include <cmath>

namespace parametric_eq {
namespace {
enum class LfoPolarity {
  bipolar = 0,
  unipolar = 1,
};

Lfo::Waveform choiceIndexToWaveform(int choiceIndex) {
  switch (choiceIndex) {
    case 1:
      return Lfo::Waveform::Triangle;
    case 2:
      return Lfo::Waveform::Square;
    case 3:
      return Lfo::Waveform::Saw;
    case 0:
    default:
      return Lfo::Waveform::Sine;
  }
}

LfoPolarity choiceIndexToPolarity(int choiceIndex) {
  return choiceIndex == static_cast<int>(LfoPolarity::unipolar)
             ? LfoPolarity::unipolar
             : LfoPolarity::bipolar;
}

void prepareLfo(Lfo& lfo, double sampleRate, int samplesPerBlock) {
  lfo.prepare({
      .sampleRate = sampleRate,
      .maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock),
      .numChannels = 1,
  });
}

float getModulatedGainDb(const BoostCutParameters& parameters, Lfo& lfo, int numSamples) {
  if (numSamples <= 0) {
    return parameters.gain.get();
  }

  lfo.setFrequency(parameters.lfo.rateHz.get());
  lfo.setWaveform(choiceIndexToWaveform(parameters.lfo.waveform.getIndex()));

  if (!parameters.lfo.enabled.get()) {
    return parameters.gain.get();
  }

  const auto depth = juce::jlimit(0.0f, 1.0f, parameters.lfo.depth.get());
  const auto baseGainDb = parameters.gain.get();

  const auto modulatedTargetGainDb =
      choiceIndexToPolarity(parameters.lfo.polarity.getIndex()) == LfoPolarity::unipolar
          ? baseGainDb * lfo.advanceAndGetLastUnipolarSample(numSamples)
          : std::abs(baseGainDb) * lfo.advanceAndGetLastSample(numSamples);

  return juce::jmap(depth, baseGainDb, modulatedTargetGainDb);
}
} // namespace

AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      ) {
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {}

const juce::String AudioPluginAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const {
  return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms() {
  return 1;
}

int AudioPluginAudioProcessor::getCurrentProgram() {
  return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram(int index) {
  juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);
  return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index,
                                                  const juce::String& newName) {
  juce::ignoreUnused(index, newName);
}

void AudioPluginAudioProcessor::prepareToPlay(double sampleRate,
                                              int samplesPerBlock) {
  auto numChannels = std::min(getTotalNumInputChannels(), getTotalNumOutputChannels());
  parametricEq_.prepare(sampleRate, numChannels);
  spectrumAnalyzer_.prepare(sampleRate, numChannels);
  for (auto& lfo : peakGainLfos_) {
    prepareLfo(lfo, sampleRate, samplesPerBlock);
  }
  prepareLfo(lowShelfGainLfo_, sampleRate, samplesPerBlock);
  prepareLfo(highShelfGainLfo_, sampleRate, samplesPerBlock);
  bypassTransitioner_.prepare({
    .sampleRate = sampleRate,
    .maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock),
    .numChannels = static_cast<juce::uint32>(
      juce::jmax(getTotalNumInputChannels(), getTotalNumOutputChannels())),
  });

}

void AudioPluginAudioProcessor::releaseResources() {
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages) {
  juce::ignoreUnused(midiMessages);

  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // This is here to avoid people getting screaming feedback
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
    buffer.clear(i, 0, buffer.getNumSamples());
  }


  bypassTransitioner_.setBypass(parameters_.bypassed.get());
  if (parameters_.bypassed.get() && !bypassTransitioner_.isTransitioning() == true) {
    return;
  }
  bypassTransitioner_.setDryBuffer(buffer);

  if (!parameters_.isPost.get()) {
    spectrumAnalyzer_.pushBlock(buffer);
  }

  for (size_t i = 0; i < ParametricEq::NUM_PEAKS; i++) {
    const auto& peak = parameters_.peakFilters[i];
    const auto modulatedGainDb =
        getModulatedGainDb(*peak, peakGainLfos_[i], buffer.getNumSamples());
    parametricEq_.setPeakParameters(
      i, 
      static_cast<double>(peak->base.frequency.get()),
      static_cast<double>(peak->base.qFactor.get()), 
      modulatedGainDb,
      peak->base.bypassed.get()
    );
  }

  const auto& lowShelf = parameters_.lowShelfParameters;
  const auto lowShelfModulatedGainDb =
      getModulatedGainDb(lowShelf, lowShelfGainLfo_, buffer.getNumSamples());
  parametricEq_.setLowShelfParameters(
    static_cast<double>(lowShelf.base.frequency.get()),
    static_cast<double>(lowShelf.base.qFactor.get()),
    lowShelfModulatedGainDb,
    lowShelf.base.bypassed.get(),
    lowShelf.base.slope.getIndex()
  );

  const auto& highShelf = parameters_.highShelfParameters;
  const auto highShelfModulatedGainDb =
      getModulatedGainDb(highShelf, highShelfGainLfo_, buffer.getNumSamples());
  parametricEq_.setHighShelfParameters(
    static_cast<double>(highShelf.base.frequency.get()),
    static_cast<double>(highShelf.base.qFactor.get()),
    highShelfModulatedGainDb,
    highShelf.base.bypassed.get(),
    highShelf.base.slope.getIndex()
  );

  const auto& lowPass = parameters_.lowPassParameters;
  parametricEq_.setLowPassParameters(
    static_cast<double>(lowPass.frequency.get()),
    static_cast<double>(lowPass.qFactor.get()),
    lowPass.bypassed.get(),
    lowPass.slope.getIndex()
  );

  const auto& highPass = parameters_.highPassParameters;
  parametricEq_.setHighPassParameters(
    static_cast<double>(highPass.frequency.get()),
    static_cast<double>(highPass.qFactor.get()),
    highPass.bypassed.get(),
    highPass.slope.getIndex()
  );

  parametricEq_.processBlock(buffer);
  bypassTransitioner_.mixToWetBuffer(buffer);

  if (parameters_.isPost.get()) {
    spectrumAnalyzer_.pushBlock(buffer);
  }
}

bool AudioPluginAudioProcessor::hasEditor() const {
  return true; 
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor() {
  return new AudioPluginAudioProcessorEditor(*this);
}

void AudioPluginAudioProcessor::getStateInformation(
    juce::MemoryBlock& destData) {
  juce::MemoryOutputStream outputStream{destData, true};
  JsonSerializer::serialize(parameters_, outputStream);
}

void AudioPluginAudioProcessor::setStateInformation(const void* data,
                                                    int sizeInBytes) {
  juce::MemoryInputStream inputStream{data, static_cast<size_t>(sizeInBytes), false};
  const auto result = JsonSerializer::deserialize(inputStream, parameters_);

  if (result.failed()) {
    DBG(result.getErrorMessage());
  }

  bypassTransitioner_.setBypassForced(parameters_.bypassed.get());
}

juce::AudioProcessorParameter* AudioPluginAudioProcessor::getBypassParameter() const {
  return &parameters_.bypassed;
}
}  // namespace parametric_eq

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new parametric_eq::AudioPluginAudioProcessor();
}
