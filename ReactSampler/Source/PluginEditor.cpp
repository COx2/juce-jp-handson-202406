#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "WebViewBundleData.h"

#ifndef WEB_VIEW_FROM_DEV_SERVER
#define WEB_VIEW_FROM_DEV_SERVER 1
#endif

namespace
{
    static const char* getMimeForExtension(const juce::String& extension)
    {
        static const std::unordered_map<juce::String, const char*> mimeMap =
        {
            { { "htm"   },  "text/html"                },
            { { "html"  },  "text/html"                },
            { { "txt"   },  "text/plain"               },
            { { "jpg"   },  "image/jpeg"               },
            { { "jpeg"  },  "image/jpeg"               },
            { { "svg"   },  "image/svg+xml"            },
            { { "ico"   },  "image/vnd.microsoft.icon" },
            { { "json"  },  "application/json"         },
            { { "png"   },  "image/png"                },
            { { "css"   },  "text/css"                 },
            { { "map"   },  "application/json"         },
            { { "js"    },  "text/javascript"          },
            { { "woff2" },  "font/woff2"               }
        };

        if (const auto it = mimeMap.find(extension.toLowerCase()); it != mimeMap.end())
            return it->second;

        jassertfalse;

        return "";
    }

    static juce::String getExtension(juce::String filename)
    {
        return filename.fromLastOccurrenceOf(".", false, false);
    }

    static auto streamToVector(juce::InputStream& stream)
    {
        std::vector<std::byte> result((size_t)stream.getTotalLength());

        stream.setPosition(0);
        
        [[maybe_unused]] const auto bytesRead = stream.read(result.data(), result.size());

        jassert(bytesRead == (juce::ssize_t)result.size());

        return result;
    }

    std::vector<std::byte> convertFromStringToByteVector(const std::string& s)
    {
        std::vector<std::byte> bytes;
        bytes.reserve(std::size(s));

        std::transform(std::begin(s), std::end(s), std::back_inserter(bytes), [](char const& c) {
            return std::byte(c);
            });

        return bytes;
    }
}

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p)
    , processorRef (p)
    , valueTreeState (p.getAPVTS())
    , misWebViewBundle(WebView::WebViewBundle_zip, WebView::WebViewBundle_zipSize, false)
{
    juce::ignoreUnused (processorRef);

    zipWebViewBundle = std::make_unique<juce::ZipFile>(misWebViewBundle);

    const auto webview_options = juce::WebBrowserComponent::Options {}
        .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
        .withKeepPageLoadedWhenBrowserIsHidden()
        .withWinWebView2Options(juce::WebBrowserComponent::Options::WinWebView2{}
        .withUserDataFolder(juce::File::getSpecialLocation(juce::File::SpecialLocationType::tempDirectory)))
        .withNativeIntegrationEnabled()
        //.withOptionsFrom(cutoffSliderRelay)
        //.withOptionsFrom(muteToggleRelay)
        //.withOptionsFrom(filterTypeComboRelay)
        .withNativeFunction("sayHelloFromWebView", [](auto& var, auto complete)
            {
                complete("Hello " + var[0].toString());
            })
        .withResourceProvider([this](const auto& url)
            {
                return getWebViewResource(url);
            },
            juce::URL{ SinglePageBrowser::localDevServerAddress }.getOrigin());

    singlePageBrowser = std::make_unique<SinglePageBrowser>(webview_options);

    addAndMakeVisible(singlePageBrowser.get());

#if WEB_VIEW_FROM_DEV_SERVER
    singlePageBrowser->goToURL(SinglePageBrowser::localDevServerAddress);
#else
    singlePageBrowser->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
#endif

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
    singlePageBrowser->setBounds (getLocalBounds());
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
        //if (juce::MessageManager::getInstance()->isThisTheMessageThread())
        //{
        //    const bool result = singlePageBrowser->evaluateJavascript (javascript.toStdString());
        //    if (! result)
        //    {
        //        juce::Logger::outputDebugString ("Failed: " + javascript);
        //    }
        //}
        //else
        //{
        //    juce::MessageManager::callAsync (
        //        [safe_this = juce::Component::SafePointer (this), javascript]
        //        {
        //            if (safe_this.getComponent() == nullptr)
        //            {
        //                return;
        //            }

        //            const bool result = safe_this->singlePageBrowser->evaluateJavascript (javascript.toStdString());
        //            if (! result)
        //            {
        //                juce::Logger::outputDebugString ("Failed: " + javascript);
        //            }
        //        });
        //}
    }
}

//==============================================================================
void AudioPluginAudioProcessorEditor::timerCallback()
{
}

//==============================================================================
std::optional<juce::WebBrowserComponent::Resource> AudioPluginAudioProcessorEditor::getWebViewResource(const juce::String& url)
{
    const auto urlToRetrive = url == "/" ? juce::String{ "index.html" }
    : url.fromFirstOccurrenceOf("/", false, false);

    if (auto* archive = zipWebViewBundle.get())
    {
        if (auto* entry = archive->getEntry(urlToRetrive))
        {
            auto stream = juce::rawToUniquePtr(archive->createStreamForEntry(*entry));
            auto v = streamToVector(*stream);
            auto mime = getMimeForExtension(getExtension(entry->filename).toLowerCase());

            return juce::WebBrowserComponent::Resource{
                std::move(v),
                std::move(mime)
            };
        }
    }

    if (urlToRetrive == "index.html")
    {
        auto fallbackIndexHtml = SinglePageBrowser::fallbackPageHtml;

        return juce::WebBrowserComponent::Resource{
            convertFromStringToByteVector(fallbackIndexHtml.toStdString()),
            juce::String { "text/html" }
        };
    }

    if (urlToRetrive == "data.txt")
    {
        juce::WebBrowserComponent::Resource resource;
        static constexpr char testData[] = "testdata";
        juce::MemoryInputStream stream{ testData, juce::numElementsInArray(testData) - 1, false };

        return juce::WebBrowserComponent::Resource{
            streamToVector(stream),
            juce::String { "text/html" }
        };
    }

    //if (urlToRetrive == "spectrumData.json")
    //{
    //    juce::Array<juce::var> frames;

    //    for (const auto& frame : spectrumDataFrames)
    //        frames.add(frame);

    //    juce::DynamicObject::Ptr d(new juce::DynamicObject());
    //    d->setProperty("timeResolutionMs", getTimerInterval());
    //    d->setProperty("frames", std::move(frames));

    //    const auto s = juce::JSON::toString(d.get());
    //    juce::MemoryInputStream stream{ s.getCharPointer(), s.getNumBytesAsUTF8(), false };

    //    return juce::WebBrowserComponent::Resource{
    //        streamToVector(stream),
    //        juce::String { "application/json" }
    //    };
    //}

    return std::nullopt;
}
