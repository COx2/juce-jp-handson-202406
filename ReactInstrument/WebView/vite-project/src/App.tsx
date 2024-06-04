// @ts-ignore
import { useState, useEffect } from 'react'
import MidiKeyboard from './components/MidiKeyboard.tsx'
import GainKnob from './components/GainKnob.tsx'
import SoundSelector from './components/SoundSelector.tsx';
import LoadSampleButton from './components/LoadSampleButton.tsx';
import { R3FScene } from './components/r3f/Scene.tsx';
// @ts-ignore
import * as Juce from "juce-framework-frontend";
import './App.css'

const customSound_Change_EventId = "onCustomSoundChanged";

function App() {
  const [customSoundName, setCustomSoundName] = useState("")

  // @ts-ignore
  const getCustomSoundName = Juce.getNativeFunction("getCustomSoundName");

  useEffect(() => {
    // @ts-ignore
    globalThis.onCustomSoundChangedId = window.__JUCE__.backend.addEventListener(
      customSound_Change_EventId,
      (args: any) =>{
        setCustomSoundName(args);
      }
    );

    (async() => {
      // @ts-ignore
      const sound_name = await getCustomSoundName();
      setCustomSoundName(sound_name);
    })();

    return function cleanup() {
      // @ts-ignore
      window.__JUCE__.backend.removeEventListener(
        // @ts-ignore
        globalThis.onCustomSoundChangedId
      );
    };
  }, [setCustomSoundName]);
  
  return (
    <>
      <div className="control-box">
        <div className="control-panel">
          <div className="control-block">
            <div className="control-label">
              SOUND
            </div>
            <div className="control-sound">
              <SoundSelector/>
            </div>
            <div className="control-sound">
              <LoadSampleButton text="Load Custom Sound"/>
            </div>
            <div style={{width: '300px'}}>
              Custom: {customSoundName}
            </div>
          </div>
          <div className="control-block">
            <div className="control-label">
              GAIN
            </div>
            <div className="force-centering">
              <GainKnob/>
            </div>
          </div>
        </div>
        <div>
          <MidiKeyboard />
        </div>
      </div>
      <div className="r3f-box">
        <R3FScene/>
      </div>
    </>
  )
}

export default App
