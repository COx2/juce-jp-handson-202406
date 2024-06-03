#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p)
    , processorRef (p)
    , valueTreeState (p.getAPVTS())
{
    juce::ignoreUnused (processorRef);

    // Native view......
    genericEditor = std::make_unique<juce::GenericAudioProcessorEditor>(processorRef);
    addAndMakeVisible(genericEditor.get());

    // Web view...
    webViewBackend = std::make_unique<WebViewBackendComponent>(processorRef);
    addAndMakeVisible(webViewBackend.get());

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (960, 600);
    setResizable(true, true);

    midiKeyboardStatePtr = processorRef.getMidiKeyboardState();
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto rect_ui = getLocalBounds();

    genericEditor->setBounds(getLocalBounds());

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    webViewBackend->setBounds (getLocalBounds());
}
