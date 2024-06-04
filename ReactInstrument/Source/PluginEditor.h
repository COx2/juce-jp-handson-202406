#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginProcessor.h"

// Forward declaration
class WebViewBackendComponent;

//==============================================================================
class AudioPluginAudioProcessorEditor final
    : public juce::AudioProcessorEditor
    , private juce::Value::Listener
{
public:
    //==============================================================================
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void openCustomSoundFileChooser();

private:
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void valueChanged(juce::Value& value) override;

    //==============================================================================
    AudioPluginAudioProcessor& processorRef;

    std::unique_ptr<juce::GenericAudioProcessorEditor> genericEditor;
    std::unique_ptr<WebViewBackendComponent> webViewBackend;
    
    std::shared_ptr<juce::MidiKeyboardState> midiKeyboardStatePtr;
    std::unique_ptr<juce::MidiKeyboardComponent> midiKeyboardComponent;

    std::unique_ptr<juce::TextButton> buttonCustomSound;
    std::unique_ptr<juce::FileChooser> soundFileChooser;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
