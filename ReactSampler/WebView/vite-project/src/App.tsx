// @ts-ignore
import { useState } from 'react'
import MidiKeyboard from './components/MidiKeyboard.tsx'
import GainKnob from './components/GainKnob.tsx'
import SoundSelector from './components/SoundSelector.tsx';
import LoadSampleButton from './components/LoadSampleButton.tsx';
// @ts-ignore
import * as Juce from "juce-framework-frontend";
import './App.css'

function App() {
  return (
    <>
      <div className="control-panel">
        <div className="control-block">
          <div className="control-label">
            SOUND
          </div>
          <div>
            <SoundSelector/>
          </div>
          <div>
            <LoadSampleButton text="Load Custom Sound"/>
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
        <MidiKeyboard/>
      </div>
    </>
  )
}

export default App
