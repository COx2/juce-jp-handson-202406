// @ts-ignore
import { React, useState, useEffect } from 'react';
// @ts-ignore
import { Knob, Pointer, Value, Arc} from 'rc-knob'
// @ts-ignore
import * as Juce from "juce-framework-frontend";

const GainKnob = (props: any) => {
  // @ts-ignore
  const sliderState = Juce.getSliderState("gainSlider");

  const [value0To1, setValue0To1] = useState(sliderState.getNormalisedValue());

  // @ts-ignore
  const [properties, setProperties] = useState(sliderState.properties);

  // @ts-ignore
  const handleChange = (event: any, newValue0To1: any) => {
    sliderState.setNormalisedValue(newValue0To1);
    setValue0To1(value0To1);
  };

  // @ts-ignore
  const mouseDown = () => {
    sliderState.sliderDragStarted();
  };

  // @ts-ignore
  const changeCommitted = (event: any, newValue0To1: any) => {
    sliderState.setNormalisedValue(newValue0To1);
    sliderState.sliderDragEnded();
  };

  useEffect(() => {
    const valueListenerId = sliderState.valueChangedEvent.addListener(() => {
      setValue0To1(sliderState.getNormalisedValue());
      props.onKnobValueChange(sliderState.getNormalisedValue());
    });
    const propertiesListenerId = sliderState.propertiesChangedEvent.addListener(
      () => setProperties(sliderState.properties)
    );
    props.onKnobValueChange(sliderState.getNormalisedValue());
    return function cleanup() {
      sliderState.valueChangedEvent.removeListener(valueListenerId);
      sliderState.propertiesChangedEvent.removeListener(propertiesListenerId);
    };
  });

  // @ts-ignore
  function calculateValue() {
    return sliderState.getScaledValue();
  }

  return (
    <Knob 
      size={110}  
      angleOffset={220} 
      angleRange={280}
      min={0}
      max={100}
      // @ts-ignore
      onChange={value => {
          handleChange("changed", value / 100)
      }}
    >
        <Arc 
        arcWidth={8}
        background="#D9D9D9"
        color="#4D4D9D"
        radius={47.5} 
        percentage={value0To1} // Refer to: https://github.com/eskimoblood/rc-knob/issues/20
        />
        <Pointer 
        width={8}
        radius={35}
        type="circle"
        color="#3d4244"
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