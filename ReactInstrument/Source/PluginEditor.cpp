#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "WebViewBackendComponent.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p)
    , processorRef (p)
    , midiKeyboardStatePtr(processorRef.getMidiKeyboardState())
{
    // Native view......
    genericEditor = std::make_unique<juce::GenericAudioProcessorEditor>(processorRef);
    addAndMakeVisible(genericEditor.get());

    buttonCustomSound = std::make_unique<juce::TextButton>("Load Custom Sound");
    buttonCustomSound->onClick = 
        [safe_this = juce::Component::SafePointer(this)]() {
            if (safe_this.getComponent() == nullptr)
            {
                return;
            }

            safe_this->openCustomSoundFileChooser();
        };
    addAndMakeVisible(buttonCustomSound.get());

    midiKeyboardComponent = std::make_unique<juce::MidiKeyboardComponent>(
        *midiKeyboardStatePtr.get(),
        juce::KeyboardComponentBase::Orientation::horizontalKeyboard);
    addAndMakeVisible(midiKeyboardComponent.get());

    // Web view...
    webViewBackend = std::make_unique<WebViewBackendComponent>(processorRef, *this);
    addAndMakeVisible(webViewBackend.get());

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1440, 960);
    setResizable(true, true);

    processorRef.getCustomSoundFileValue().addListener(this);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    processorRef.getCustomSoundFileValue().removeListener(this);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::openCustomSoundFileChooser()
{
    const juce::String message = "Please select a custom sound file.";
    const juce::String wild_card = processorRef.getSamplerSupportedFormatWildcard();

    soundFileChooser = std::make_unique<juce::FileChooser>(message,
        juce::File(),
        wild_card,
        true,
        false,
        this);

    soundFileChooser->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [safe_this = juce::Component::SafePointer(this)](const juce::FileChooser& chooser)
        {
            if (safe_this.getComponent() == nullptr)
            {
                return;
            }

            const juce::File sound_file = chooser.getResult();
            juce::Logger::outputDebugString(sound_file.getFullPathName());

            if (sound_file.existsAsFile())
            {
                safe_this->processorRef.loadCustomSoundFile(sound_file);
            }
        });
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto rect_ui = getLocalBounds();

#if 0
    auto rect_juce_ui = rect_ui.removeFromRight(rect_ui.getWidth() * 0.5f);
    {
        const auto height_generic = rect_juce_ui.getHeight() * 0.65f;
        const auto height_button = rect_juce_ui.getHeight() * 0.15f;
        genericEditor->setBounds(rect_juce_ui.removeFromTop(height_generic));
        buttonCustomSound->setBounds(rect_juce_ui.removeFromTop(height_button)
            .withSizeKeepingCentre(rect_juce_ui.getWidth() * 0.65f, height_button * 0.75f));
        midiKeyboardComponent->setBounds(rect_juce_ui);
    }
#endif

    auto rect_web_ui = rect_ui;

    webViewBackend->setBounds (rect_web_ui);
}

void AudioPluginAudioProcessorEditor::valueChanged(juce::Value& value)
{
    if (value.refersToSameSourceAs(processorRef.getCustomSoundFileValue()))
    {
        juce::var file_name = juce::File(value.getValue()).getFileName();
        webViewBackend->emitCustomSoundChangedEvent(file_name);
    }
}
