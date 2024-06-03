import Button from '@mui/material/Button';
// @ts-ignore
import * as Juce from "juce-framework-frontend";

const OpenFileChooserButton = (props: any) => {
    
    const onButtonClicked = Juce.getNativeFunction("onLoadCustomSound");

    return (
        <Button
        variant="contained"
        onClick={() => {
            onButtonClicked(props.native_message);
        }}
        >
            {props.text}
        </Button>
    );
};

export default OpenFileChooserButton;