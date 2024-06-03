import { useState } from 'react'
import MidiKeyboard from './components/MidiKeyboard.tsx'
import GainKnob from './components/GainKnob.tsx'
import SoundSelector from './components/SoundSelector.tsx';
import OpenFileChooserButton from './components/OpenFileChooserButton.tsx';
// @ts-ignore
import * as Juce from "juce-framework-frontend";
import './App.css'

function App() {
  return (
    <>
      <h1>React Sampler</h1>
      <div style={{display: 'flex', flexDirection: 'column', justifyContent:'center', alignItems:'center'}}>
          <div style={{marginTop: '-10px'}}><GainKnob/></div>
          <div style={{marginTop: '-10px'}}>GAIN</div>
          <div style={{margin: '10px', padding: 0}}></div>
      </div>
      <div style={{display: 'flex', flexDirection: 'column', justifyContent:'center', alignItems:'center'}}>
          <div style={{marginTop: '0px'}}>SOUND</div>
          <div style={{marginTop: '10px'}}><SoundSelector/></div>
          <div style={{marginTop: '10px'}}><OpenFileChooserButton text="Load Sample"/></div>
          <div style={{margin: '10px', padding: 0}}></div>
      </div>
      <div>
        <MidiKeyboard/>
      </div>
    </>
  )
}

export default App
