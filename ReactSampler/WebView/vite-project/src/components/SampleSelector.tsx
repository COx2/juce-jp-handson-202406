import { useState } from 'react';
import Select from '@jsteinotr/react-select';

const options = [
  { value: 'chocolate', label: 'Chocolate' },
  { value: 'strawberry', label: 'Strawberry' },
  { value: 'vanilla', label: 'Vanilla' },
];

const SampleSelector  = () => {
  const [selectedOption, setSelectedOption] = useState(null);

  return (
    <Select 
        defaultValue={selectedOption}
        onChange={setSelectedOption}
        options={options}
        // Refer to: https://stackoverflow.com/questions/55830799/how-to-change-zindex-in-react-select-drowpdown
        styles={{
          // Fixes the overlapping problem of the component
          menu: provided => ({ ...provided, zIndex: 9999 })
        }}
    />
  );
}

export default SampleSelector;
