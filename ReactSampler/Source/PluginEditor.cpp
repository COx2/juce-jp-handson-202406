#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p)
    , processorRef (p)
    , valueTreeState (p.getAPVTS())
    , misWebViewBundle (WebView::WebViewBundle_zip, WebView::WebViewBundle_zipSize, false)
{
    juce::ignoreUnused (processorRef);

    zipWebViewBundle = std::make_unique<juce::ZipFile> (misWebViewBundle);
    
    gainLabel.setText ("Gain", juce::dontSendNotification);
    addAndMakeVisible (gainLabel);

    addAndMakeVisible (gainSlider);
    gainAttachment.reset (new SliderAttachment (valueTreeState, "gain", gainSlider));
    
    invertButton.setButtonText ("Invert Phase");
    addAndMakeVisible (invertButton);
    invertAttachment.reset (new ButtonAttachment (valueTreeState, "invertPhase", invertButton));

    // Parameter binding.
    valueTreeState.addParameterListener("gain", this);
    valueTreeState.addParameterListener("invertPhase", this);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (960, 480);
    setResizable(true, true);

    //startTimerHz (30);

    midiKeyboardStatePtr = processorRef.getMidiKeyboardState();
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    valueTreeState.removeParameterListener("gain", this);
    valueTreeState.removeParameterListener("invertPhase", this);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto rect_ui = getLocalBounds();

    auto gainRect = rect_ui.removeFromTop (200);
    gainLabel .setBounds (gainRect.removeFromLeft (200));
    gainSlider.setBounds (gainRect);

    invertButton.setBounds (rect_ui.removeFromTop (200));

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    juceWebViewHolder->setBounds (getLocalBounds());
}

void AudioPluginAudioProcessorEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    juce::String javascript = "";

    if (parameterID == "gain")
    {
        juce::DynamicObject::Ptr json = new juce::DynamicObject();
        json->setProperty("parameterName", "gain");
        json->setProperty("parameterValue", newValue);
        const auto js_args_json = juce::JSON::toString(json.get());
        javascript = juce::String ("onParameterChanged(") + js_args_json + juce::String (")");
    }
    else if (parameterID == "invertPhase")
    {
        juce::DynamicObject::Ptr json = new juce::DynamicObject();
        json->setProperty("parameterName", "invertPhase");
        json->setProperty("parameterValue", newValue);
        const auto js_args_json = juce::JSON::toString(json.get());
        javascript = juce::String ("onParameterChanged(") + js_args_json + juce::String (")");
    }

    if (javascript.isNotEmpty())
    {
        if (juce::MessageManager::getInstance()->isThisTheMessageThread())
        {
            const bool result = webView->evaluateJavascript (javascript.toStdString());
            if (! result)
            {
                juce::Logger::outputDebugString ("Failed: " + javascript);
            }
        }
        else
        {
            juce::MessageManager::callAsync (
                [safe_this = juce::Component::SafePointer (this), javascript]
                {
                    if (safe_this.getComponent() == nullptr)
                    {
                        return;
                    }

                    const bool result = safe_this->webView->evaluateJavascript (javascript.toStdString());
                    if (! result)
                    {
                        juce::Logger::outputDebugString ("Failed: " + javascript);
                    }
                });
        }
    }
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
}
