// @ts-ignore
import { React, useState, useEffect } from 'react';
// @ts-ignore
import { Knob, Pointer, Value, Arc} from 'rc-knob'
// @ts-ignore
import * as Juce from "juce-framework-frontend";

const GainKnob = () => {
  // @ts-ignore
  const sliderState = Juce.getSliderState("gainSlider");

  const [value0To1, setValue0To1] = useState(sliderState.getNormalisedValue());
  const [properties, setProperties] = useState(sliderState.properties);

  const handleChange = (event: any, newValue0To1: any) => {
    sliderState.setNormalisedValue(newValue0To1);
    setValue0To1(value0To1);
  };

  const mouseDown = () => {
    sliderState.sliderDragStarted();
  };

  const changeCommitted = (event: any, newValue0To1: any) => {
    sliderState.setNormalisedValue(newValue0To1);
    sliderState.sliderDragEnded();
  };

  useEffect(() => {
    const valueListenerId = sliderState.valueChangedEvent.addListener(() => {
      setValue0To1(sliderState.getNormalisedValue());
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
        handleChange("changed", value / 100)
    }}
    >
        <Arc 
        arcWidth={8}
        color="#FC5A96"
        background="#180094"
        radius={47.5} 
        percentage={value0To1} // Refer to: https://github.com/eskimoblood/rc-knob/issues/20
        />
        <Pointer 
        width={8}
        radius={35}
        type="circle"
        color="000000"
        percentage={value0To1} // Refer to: https://github.com/eskimoblood/rc-knob/issues/20
        />
        <Value 
        marginBottom={40} 
        className="value"
        value={value0To1 * 100} // Refer to: https://github.com/eskimoblood/rc-knob/issues/20
        />
    </Knob>
  );
};

export default GainKnob;