#include "JuceDemoSynthesizer.h"

namespace
{
//==============================================================================
/** A demo synth sound that's just a basic sine wave.. */
class SineWaveSound final : public juce::SynthesiserSound
{
public:
    SineWaveSound() {}

    bool appliesToNote (int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel (int /*midiChannel*/) override { return true; }
};

//==============================================================================
/** A simple demo synth voice that just plays a sine wave.. */
class SineWaveVoice final : public juce::SynthesiserVoice
{
public:
    SineWaveVoice() {}

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/) override
    {
        currentAngle = 0.0;
        level = velocity * 0.15;
        tailOff = 0.0;

        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();

        angleDelta = cyclesPerSample * juce::MathConstants<double>::twoPi;
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            // start a tail-off by setting this flag. The render callback will pick up on
            // this and do a fade out, calling clearCurrentNote() when it's finished.

            if (juce::approximatelyEqual (tailOff, 0.0)) // we only need to begin a tail-off if it's not already doing so - the
                // stopNote method could be called more than once.
                tailOff = 1.0;
        }
        else
        {
            // we're being told to stop playing immediately, so reset everything..

            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

    void pitchWheelMoved (int /*newValue*/) override
    {
        // not implemented for the purposes of this demo!
    }

    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override
    {
        // not implemented for the purposes of this demo!
    }

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (! juce::approximatelyEqual (angleDelta, 0.0))
        {
            if (tailOff > 0.0)
            {
                while (--numSamples >= 0)
                {
                    auto currentSample = (float) (sin (currentAngle) * level * tailOff);

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;

                    tailOff *= 0.99;

                    if (tailOff <= 0.005)
                    {
                        // tells the synth that this voice has stopped
                        clearCurrentNote();

                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0)
                {
                    auto currentSample = (float) (sin (currentAngle) * level);

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;
                }
            }
        }
    }

    using SynthesiserVoice::renderNextBlock;

private:
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    double level = 0.0;
    double tailOff = 0.0;
};
}

//==============================================================================
// JuceDemoSynthesizer
//==============================================================================
JuceDemoSynthesizer::JuceDemoSynthesizer()
{
    audioFormatManager.registerBasicFormats();
}

JuceDemoSynthesizer::~JuceDemoSynthesizer()
{
}

void JuceDemoSynthesizer::loadSineWave()
{
    clearVoices();
    clearSounds();

    const auto numVoices = 8;

    // Add some voices...
    for (int i = 0; i < numVoices; ++i)
    {
        addVoice(new SineWaveVoice());
    }

    // ..and give the synth a sound to play
    addSound(new SineWaveSound());
}

void JuceDemoSynthesizer::loadAudioSample(std::unique_ptr<juce::InputStream> audioFileStream)
{
    audioFormatReader.reset(audioFormatManager.createReaderFor(std::move(audioFileStream)));
    if (audioFormatReader.get() != nullptr)
    {
        clearVoices();
        clearSounds();

#if 0
        // Monophonic
        //addVoice(new juce::SamplerVoice());
#else
        // Polyphonic
        const auto numVoices = 8;
        for (int i = 0; i < numVoices; i++) {
            addVoice(new juce::SamplerVoice());
        }
#endif

        // ..and give the synth a sound to play
        juce::BigInteger allNotes;
        allNotes.setRange(0, 128, true);

        addSound(new juce::SamplerSound("default", *audioFormatReader.get(), allNotes, 60, 0, 0.1, 10.0));
    }

}

juce::String JuceDemoSynthesizer::getWildCardFilter() const
{
    return audioFormatManager.getWildcardForAllFormats();
}
