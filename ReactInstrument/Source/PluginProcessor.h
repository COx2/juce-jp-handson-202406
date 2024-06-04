#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "JuceDemoSynthesizer.h"

//==============================================================================
class AudioPluginAudioProcessor final
    : public juce::AudioProcessor
    , private juce::Value::Listener
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    std::shared_ptr<juce::MidiKeyboardState> getMidiKeyboardState() const;
    juce::AudioProcessorValueTreeState& getAPVTS() { return *parameters.get(); }

    //==============================================================================
    juce::String getSamplerSupportedFormatWildcard() const;
    void loadCustomSoundFile(const juce::File& fileToLoad);

private:
    //==============================================================================
    void valueChanged(juce::Value& value) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    std::unique_ptr<juce::AudioProcessorValueTreeState> parameters;

    std::unique_ptr<JuceDemoSynthesizer> sineWaveSynthesizer;
    std::unique_ptr<JuceDemoSynthesizer> customSamplerSynthesizer;
    std::shared_ptr<juce::MidiKeyboardState> midiKeyboardState;

    float previousGain;
    std::atomic<float>* gainParameter = nullptr;
    std::atomic<float>* phaseParameter = nullptr;
    std::atomic<float>* soundSelectorParameter = nullptr;

    juce::Value customSoundFilePath;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
