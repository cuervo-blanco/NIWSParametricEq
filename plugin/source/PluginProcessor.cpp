#include "SimpleParametricEq/PluginProcessor.h"
#include "SimpleParametricEq/PluginEditor.h"

namespace parametric_eq {
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
  juce::ignoreUnused(samplesPerBlock);
  auto numChannels = std::min(getTotalNumInputChannels(), getTotalNumOutputChannels());
  parametricEq_.prepare(sampleRate, numChannels);
  spectrumAnalyzer_.prepare(sampleRate, numChannels);
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
    parametricEq_.setPeakParameters(
      i, 
      static_cast<double>(peak->base.frequency.get()),
      static_cast<double>(peak->base.qFactor.get()), 
      peak->gain.get(), 
      peak->base.bypassed.get()
    );
  }

  const auto& lowShelf = parameters_.lowShelfParameters;
  parametricEq_.setLowShelfParameters(
    static_cast<double>(lowShelf.base.frequency.get()),
    static_cast<double>(lowShelf.base.qFactor.get()),
    lowShelf.gain.get(),
    lowShelf.base.bypassed.get()
  );

  const auto& lowPass = parameters_.lowPassParameters;
  parametricEq_.setLowPassParameters(
    static_cast<double>(lowPass.frequency.get()),
    static_cast<double>(lowPass.qFactor.get()),
    lowPass.bypassed.get()
  );

  const auto& highPass = parameters_.highPassParameters;
  parametricEq_.setHighPassParameters(
    static_cast<double>(highPass.frequency.get()),
    static_cast<double>(highPass.qFactor.get()),
    highPass.bypassed.get()
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
  juce::ignoreUnused(destData);
}

void AudioPluginAudioProcessor::setStateInformation(const void* data,
                                                    int sizeInBytes) {
  juce::ignoreUnused(data, sizeInBytes);
}

juce::AudioProcessorParameter* AudioPluginAudioProcessor::getBypassParameter() const {
  return &parameters_.bypassed;
}
}  // namespace parametric_eq

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new parametric_eq::AudioPluginAudioProcessor();
}
