import { useState, useEffect } from 'react';
import Select from '@jsteinotr/react-select';
// @ts-ignore
import * as Juce from "juce-framework-frontend";

const SoundSelector  = () => {
  const comboBoxState = Juce.getComboBoxState("soundSelector");

  const [choiceIndex, setChoiceIndex] = useState(comboBoxState.getChoiceIndex());
  const [properties, setProperties] = useState(comboBoxState.properties);

  const handleChange = (event: any) => {
    comboBoxState.setChoiceIndex(event.value);
    setChoiceIndex(event.value);
  };

  useEffect(() => {
    const valueListenerId = comboBoxState.valueChangedEvent.addListener(() => {
      setChoiceIndex(comboBoxState.getChoiceIndex());
    });
    const propertiesListenerId =
      comboBoxState.propertiesChangedEvent.addListener(() => {
        setProperties(comboBoxState.properties);
      });

    return function cleanup() {
      comboBoxState.valueChangedEvent.removeListener(valueListenerId);
      comboBoxState.propertiesChangedEvent.removeListener(propertiesListenerId);
    };
  });

  const options = properties.choices.map((element: any, index: number) => {
      return { value: index, label: element };
  });

  return (
    <Select 
        defaultValue={options[choiceIndex]}
        onChange={handleChange}
        options={options}
        isSearchable={false}
        isClearable={false}
        // Refer to: https://stackoverflow.com/questions/55830799/how-to-change-zindex-in-react-select-drowpdown
        styles={{
          // Fixes the overlapping problem of the component
          menu: provided => ({ ...provided, zIndex: 9999 })
        }}
    />
  );
}

export default SoundSelector;
