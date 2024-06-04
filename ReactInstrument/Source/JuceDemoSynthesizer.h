#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>

//==============================================================================
class JuceDemoSynthesizer
    : public juce::Synthesiser
{
public:
    //==============================================================================
    JuceDemoSynthesizer();
    ~JuceDemoSynthesizer() override;

    //==============================================================================
    void loadSineWave();
    void loadAudioSample(std::unique_ptr<juce::InputStream> audioFileStream);
    juce::String getWildCardFilter() const;

private:
    juce::AudioFormatManager audioFormatManager;
    std::unique_ptr<juce::AudioFormatReader> audioFormatReader;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceDemoSynthesizer)
};