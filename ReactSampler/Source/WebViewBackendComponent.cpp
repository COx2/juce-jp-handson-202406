#include "WebViewBackendComponent.h"
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

std::optional<juce::WebBrowserComponent::Resource> WebViewBackendComponent::getResource(const juce::String& url)
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

WebViewBackendComponent::WebViewBackendComponent(AudioPluginAudioProcessor& processor)
    : misWebViewBundle(WebView::WebViewBundle_zip, WebView::WebViewBundle_zipSize, false)
    , zipWebViewBundle(std::make_unique<juce::ZipFile>(misWebViewBundle))
    , processorRef(processor)
    , gainAttachment(
        *processorRef.getAPVTS().getParameter("gain"),
        gainSliderRelay,
        processorRef.getAPVTS().undoManager)
    , midiKeyboardStatePtr(processorRef.getMidiKeyboardState())
{
    addAndMakeVisible(webComponent);

#if WEB_VIEW_FROM_DEV_SERVER
    webComponent.goToURL(SinglePageBrowser::localDevServerAddress);
#else
    webComponent.goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
#endif
}

WebViewBackendComponent::~WebViewBackendComponent()
{
}

void WebViewBackendComponent::paint(juce::Graphics&)
{
}

void WebViewBackendComponent::resized()
{
    webComponent.setBounds(getLocalBounds());
}

void WebViewBackendComponent::timerCallback()
{
}
