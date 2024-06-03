#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "JuceDemoSynthesizer.h"

//==============================================================================
class AudioPluginAudioProcessor final
    : public juce::AudioProcessor
    , private juce::AudioProcessorValueTreeState::Listener
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

    void openCustomSoundFileChooser();

private:
    //==============================================================================
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    std::unique_ptr<juce::AudioProcessorValueTreeState> parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    float previousGain;
 
    std::atomic<float>* phaseParameter = nullptr;
    std::atomic<float>* gainParameter  = nullptr;
    std::atomic<int> soundSelector;

    std::unique_ptr<JuceDemoSynthesizer> sineWaveSynthesizer;
    std::unique_ptr<JuceDemoSynthesizer> customSamplerSynthesizer;
    std::shared_ptr<juce::MidiKeyboardState> midiKeyboardState;
    std::unique_ptr<juce::FileChooser> soundFileChooser;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
