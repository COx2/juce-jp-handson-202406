#include "SinglePageBrowser.h"
#include "WebViewBundleData.h"

#ifndef WEB_VIEW_FROM_SERVER
    #define WEB_VIEW_FROM_SERVER 0
#endif

static ZipFile* getZipFile()
{
    misWebViewBundle (WebView::WebViewBundle_zip, WebView::WebViewBundle_zipSize, false)
    zipWebViewBundle = std::make_unique<juce::ZipFile> (misWebViewBundle);

    static auto stream = createAssetInputStream ("webviewplugin-gui_1.0.0.zip", AssertAssetExists::no);

    if (stream == nullptr)
        return nullptr;

    static ZipFile f { stream.get(), false };
    return &f;
}

static const char* getMimeForExtension (const String& extension)
{
    static const std::unordered_map<String, const char*> mimeMap =
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

    if (const auto it = mimeMap.find (extension.toLowerCase()); it != mimeMap.end())
        return it->second;

    jassertfalse;
    return "";
}

static String getExtension (String filename)
{
    return filename.fromLastOccurrenceOf (".", false, false);
}

static auto streamToVector (InputStream& stream)
{
    std::vector<std::byte> result ((size_t) stream.getTotalLength());
    stream.setPosition (0);
    [[maybe_unused]] const auto bytesRead = stream.read (result.data(), result.size());
    jassert (bytesRead == (ssize_t) result.size());
    return result;
}

std::optional<WebBrowserComponent::Resource> WebViewPluginAudioProcessorEditor::getResource (const String& url)
{
    const auto urlToRetrive = url == "/" ? String { "index.html" }
                                         : url.fromFirstOccurrenceOf ("/", false, false);

    if (auto* archive = getZipFile())
    {
        if (auto* entry = archive->getEntry (urlToRetrive))
        {
            auto stream = rawToUniquePtr (archive->createStreamForEntry (*entry));
            auto v = streamToVector (*stream);
            auto mime = getMimeForExtension (getExtension (entry->filename).toLowerCase());
            return WebBrowserComponent::Resource { std::move (v),
                                                   std::move (mime) };
        }
    }

    if (urlToRetrive == "index.html")
    {
        auto fallbackIndexHtml = createAssetInputStream ("webviewplugin-gui-fallback.html");
        return WebBrowserComponent::Resource { streamToVector (*fallbackIndexHtml),
                                               String { "text/html" } };
    }

    if (urlToRetrive == "data.txt")
    {
        WebBrowserComponent::Resource resource;
        static constexpr char testData[] = "testdata";
        MemoryInputStream stream { testData, numElementsInArray (testData) - 1, false };
        return WebBrowserComponent::Resource { streamToVector (stream), String { "text/html" } };
    }

    if (urlToRetrive == "spectrumData.json")
    {
        Array<var> frames;

        for (const auto& frame : spectrumDataFrames)
            frames.add (frame);

        DynamicObject::Ptr d (new DynamicObject());
        d->setProperty ("timeResolutionMs", getTimerInterval());
        d->setProperty ("frames", std::move (frames));

        const auto s = JSON::toString (d.get());
        MemoryInputStream stream { s.getCharPointer(), s.getNumBytesAsUTF8(), false };
        return WebBrowserComponent::Resource { streamToVector (stream), String { "application/json" } };
    }

    return std::nullopt;
}

#if JUCE_ANDROID
// The localhost is available on this address to the emulator
const String localDevServerAddress = "http://10.0.2.2:3000/";
#else
const String localDevServerAddress = "http://localhost:3000/";
#endif

bool SinglePageBrowser::pageAboutToLoad (const String& newURL)
{
    return newURL == localDevServerAddress || newURL == getResourceProviderRoot();
}

