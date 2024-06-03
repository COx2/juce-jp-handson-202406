#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

extern const String localDevServerAddress;

std::optional<WebBrowserComponent::Resource> getResource (const String& url);

//==============================================================================
struct SinglePageBrowser
    : public juce::WebBrowserComponent
{
    using WebBrowserComponent::WebBrowserComponent;

    // Prevent page loads from navigating away from our single page web app
    bool pageAboutToLoad (const String& newURL) override;
};
