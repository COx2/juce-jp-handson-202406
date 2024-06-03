// @ts-ignore
import { React, useState, useEffect } from 'react';
// @ts-ignore
import { Knob, Pointer, Value, Arc} from 'rc-knob'
// @ts-ignore
import * as Juce from "juce-framework-frontend";

const GainKnob = () => {
  // @ts-ignore
  const sliderState = Juce.getSliderState("gainSlider");

  const [value, setValue] = useState(sliderState.getNormalisedValue());
  const [properties, setProperties] = useState(sliderState.properties);

  const handleChange = (event: any, newValue: any) => {
    sliderState.setNormalisedValue(newValue / 100);
    setValue(newValue);
  };

  const mouseDown = () => {
    sliderState.sliderDragStarted();
  };

  const changeCommitted = (event: any, newValue: any) => {
    sliderState.setNormalisedValue(newValue / 100);
    sliderState.sliderDragEnded();
  };

  useEffect(() => {
    const valueListenerId = sliderState.valueChangedEvent.addListener(() => {
      setValue(sliderState.getNormalisedValue() * 100);
    });
    const propertiesListenerId = sliderState.propertiesChangedEvent.addListener(
      () => setProperties(sliderState.properties)
    );

    return function cleanup() {
      sliderState.valueChangedEvent.removeListener(valueListenerId);
      sliderState.propertiesChangedEvent.removeListener(propertiesListenerId);
    };
  });

  function calculateValue() {
    return sliderState.getScaledValue();
  }

  return (
    <Knob 
    size={100}  
    angleOffset={220} 
    angleRange={280}
    min={0}
    max={100}
    // @ts-ignore
    onChange={value => {
        const jsonData = {
            sliderName: "gain",
            sliderValue: value,
            sliderRangeMin: 0,
            sliderRangeMax: 100
        };
        handleChange("hoge", value)
    }}
    >
        <Arc 
        arcWidth={5}
        color="#FC5A96"
        background="#180094"
        radius={47.5} 
        percentage={value / 100} // Refer to: https://github.com/eskimoblood/rc-knob/issues/20
        />
        <Pointer 
        width={5}
        radius={40}
        type="circle"
        color="000000"
        percentage={value / 100} // Refer to: https://github.com/eskimoblood/rc-knob/issues/20
        />
        <Value 
        marginBottom={40} 
        className="value"
        value={value} // Refer to: https://github.com/eskimoblood/rc-knob/issues/20
        />
    </Knob>
  );
};

export default GainKnob;