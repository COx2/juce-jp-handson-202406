#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "SinglePageBrowser.h"
#include "PluginProcessor.h"

//==============================================================================
class WebViewBackendComponent
    : public juce::Component
    , private juce::Timer
{
public:
    explicit WebViewBackendComponent(AudioPluginAudioProcessor& processor);
    ~WebViewBackendComponent() override;

private:
    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void timerCallback() override;

    //==============================================================================
    std::optional<juce::WebBrowserComponent::Resource> getResource(const juce::String& url);


    //==============================================================================
    AudioPluginAudioProcessor& processorRef;

    juce::MemoryInputStream misWebViewBundle;
    std::unique_ptr<juce::ZipFile> zipWebViewBundle;

    juce::WebSliderRelay gainSliderRelay{ webComponent, "gainSlider" };

    SinglePageBrowser webComponent{ 
        juce::WebBrowserComponent::Options{}
        .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
        .withWinWebView2Options(juce::WebBrowserComponent::Options::WinWebView2{}
        .withUserDataFolder(juce::File::getSpecialLocation(juce::File::SpecialLocationType::tempDirectory)))
        .withNativeIntegrationEnabled()
        .withOptionsFrom(gainSliderRelay)
        .withNativeFunction("onMidiNoteOn", 
            [safe_this = juce::Component::SafePointer(this)]
            (const juce::Array<juce::var>& args, std::function<void(juce::var)> complete)
            -> void
            {
                if (safe_this.getComponent() == nullptr)
                {
                    complete(juce::var(-1));
                    return;
                }

                const auto midi_channel = args.getReference(0);
                const auto midi_note_number = args.getReference(1);

                if (!safe_this->midiKeyboardStatePtr.expired())
                {
                    safe_this->midiKeyboardStatePtr.lock()->noteOn((int)midi_channel, (int)midi_note_number, 0.6f);
                }

                complete(juce::var(0));
                return;
            })
        .withNativeFunction("onMidiNoteOff", 
            [safe_this = juce::Component::SafePointer(this)]
            (const juce::Array<juce::var>& args, std::function<void(juce::var)> complete)
            -> void
            {
                if (safe_this.getComponent() == nullptr)
                {
                    complete(juce::var(-1));
                    return;
                }

                const auto midi_channel = args.getReference(0);
                const auto midi_note_number = args.getReference(1);

                if (!safe_this->midiKeyboardStatePtr.expired())
                {
                    safe_this->midiKeyboardStatePtr.lock()->noteOff((int)midi_channel, (int)midi_note_number, 0.0f);
                }

                complete(juce::var(0));
                return;
            })
        .withResourceProvider([this](const auto& url)
        {
            return getResource(url);
        },
        juce::URL { SinglePageBrowser::localDevServerAddress }.getOrigin())
    };

    juce::WebSliderParameterAttachment gainAttachment;

    std::weak_ptr<juce::MidiKeyboardState> midiKeyboardStatePtr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WebViewBackendComponent)
};
