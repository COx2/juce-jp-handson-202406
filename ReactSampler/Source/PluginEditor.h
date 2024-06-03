#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_data_structures/juce_data_structures.h>
#include "PluginProcessor.h"
#include "SinglePageBrowser.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final
    : public juce::AudioProcessorEditor
    , private juce::AudioProcessorValueTreeState::Listener
    , private juce::ValueTree::Listener
    , private juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    virtual void parameterChanged(const juce::String& parameterID, float newValue) override;

    virtual void timerCallback() override;

    std::optional<juce::WebBrowserComponent::Resource> getWebViewResource(const juce::String& url);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    std::weak_ptr<juce::MidiKeyboardState> midiKeyboardStatePtr;

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    
    juce::AudioProcessorValueTreeState& valueTreeState;
 
    juce::Label gainLabel;
    juce::Slider gainSlider;
    std::unique_ptr<SliderAttachment> gainAttachment;
 
    juce::ToggleButton invertButton;
    std::unique_ptr<ButtonAttachment> invertAttachment;

    std::unique_ptr<SinglePageBrowser> singlePageBrowser;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
