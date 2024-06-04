#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
    , customSoundFilePath(juce::var(""))
{
    sineWaveSynthesizer = std::make_unique<JuceDemoSynthesizer>();
    sineWaveSynthesizer->loadSineWave();

    customSamplerSynthesizer = std::make_unique<JuceDemoSynthesizer>();

    midiKeyboardState = std::make_shared<juce::MidiKeyboardState>();

    parameters = std::make_unique<juce::AudioProcessorValueTreeState>(
        *this, 
        nullptr, 
        juce::Identifier ("PluginParameters"),
        createParameterLayout());
    
    phaseParameter = parameters->getRawParameterValue ("invertPhase");
    gainParameter  = parameters->getRawParameterValue ("gain");
    soundSelectorParameter = parameters->getRawParameterValue("soundSelector");

    customSoundFilePath.addListener(this);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    customSoundFilePath.removeListener(this);
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);

    sineWaveSynthesizer->setCurrentPlaybackSampleRate (sampleRate);
    customSamplerSynthesizer->setCurrentPlaybackSampleRate(sampleRate);

    midiKeyboardState->reset();

    auto phase = *phaseParameter < 0.5f ? 1.0f : -1.0f;
    previousGain = *gainParameter * phase;
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto phase = *phaseParameter < 0.5f ? 1.0f : -1.0f;
    auto currentGain = *gainParameter * phase;

    // pass these messages to the keyboard state so that it can update the component
    // to show on-screen which keys are being pressed on the physical midi keyboard.
    // This call will also add midi messages to the buffer which were generated by
    // the mouse-clicking on the on-screen keyboard.
    midiKeyboardState->processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);

    auto buffer_sine_wave = juce::AudioBuffer<float>(buffer.getNumChannels(), buffer.getNumSamples());
    buffer_sine_wave.clear();

    auto buffer_csutom_sampler = juce::AudioBuffer<float>(buffer.getNumChannels(), buffer.getNumSamples());
    buffer_csutom_sampler.clear();

    sineWaveSynthesizer->renderNextBlock(buffer_sine_wave, midiMessages, 0, buffer_sine_wave.getNumSamples());
    customSamplerSynthesizer->renderNextBlock(buffer_csutom_sampler, midiMessages, 0, buffer_csutom_sampler.getNumSamples());

    if ((int)*soundSelectorParameter == 0)
    {
        buffer = buffer_sine_wave;
    }
    else if ((int)*soundSelectorParameter == 1)
    {
        buffer = buffer_csutom_sampler;
    }

    if (juce::approximatelyEqual (currentGain, previousGain))
    {
        buffer.applyGain (currentGain);
    }
    else
    {
        buffer.applyGainRamp (0, buffer.getNumSamples(), previousGain, currentGain);
        previousGain = currentGain;
    }

    midiMessages.clear();
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);

    auto xml = std::make_unique<juce::XmlElement>("ReactInstrument");
    {
        auto state = parameters->copyState();
        xml->addChildElement(state.createXml().release());
    }
    {
        auto state = std::make_unique<juce::XmlElement>("SamplerSettings");
        state->setAttribute("CustomSoundFilePath", customSoundFilePath.toString());
        xml->addChildElement(state.release());
    }
    juce::Logger::outputDebugString(xml->toString());
    copyXmlToBinary (*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
    
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr && xmlState->hasTagName("ReactInstrument"))
    {
        if (const auto* xml_params = xmlState->getChildByName(parameters->state.getType()))
        {
            parameters->replaceState (juce::ValueTree::fromXml (*xml_params));
        }

        if (const auto * xml_sampler = xmlState->getChildByName("SamplerSettings"))
        {
            const auto value = xml_sampler->getStringAttribute("CustomSoundFilePath");
            customSoundFilePath = value;
        }
    }
}

//==============================================================================
std::shared_ptr<juce::MidiKeyboardState> AudioPluginAudioProcessor::getMidiKeyboardState() const
{
    return midiKeyboardState;
}

juce::String AudioPluginAudioProcessor::getSamplerSupportedFormatWildcard() const
{
    return customSamplerSynthesizer->getWildCardFilter();
}

void AudioPluginAudioProcessor::loadCustomSoundFile(const juce::File& fileToLoad)
{
    customSoundFilePath = fileToLoad.getFullPathName();
}

void AudioPluginAudioProcessor::valueChanged(juce::Value& value)
{
    if (value.refersToSameSourceAs(customSoundFilePath))
    {
        juce::File file_to_load = juce::File(customSoundFilePath.toString());
        if (file_to_load.existsAsFile())
        {
            customSamplerSynthesizer->loadAudioSample(file_to_load.createInputStream());
        }
    }
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    // It will move should not const.
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "gain", 1 },
          "Gain",
          juce::NormalisableRange<float>(0.0f, 1.0f),
          0.8f));

    parameters.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ "invertPhase", 1 },
           "Invert Phase",
            false));

    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "soundSelector", 1 },
        "Sound Selector", 
        juce::StringArray{ "Sine Wave", "Custom Sampler" }, 
        0));

    return juce::AudioProcessorValueTreeState::ParameterLayout{ parameters.begin(), parameters.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
