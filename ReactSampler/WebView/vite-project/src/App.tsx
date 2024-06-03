import { useState } from 'react'
import MidiKeyboard from './components/MidiKeyboard.tsx'
import GainKnob from './components/GainKnob.tsx'
import SampleSelector from './components/SampleSelector.tsx';
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
          <div style={{marginTop: '0px'}}>SAMPLE</div>
          <div style={{marginTop: '10px'}}><SampleSelector/></div>
          <div style={{margin: '10px', padding: 0}}></div>
      </div>
      <div>
        <MidiKeyboard/>
      </div>
    </>
  )
}

export default App
