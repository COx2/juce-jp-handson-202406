import Button from '@mui/material/Button';
// @ts-ignore
import * as Juce from "juce-framework-frontend";

const LoadSampleButton = (props: any) => {
    const onButtonClicked = Juce.getNativeFunction("onLoadCustomSound");

    return (
        <Button
        variant="contained"
        sx={{
            width: '240px'
        }}
        onClick={() => {
            onButtonClicked(props.native_message);
        }}
        >
            {props.text}
        </Button>
    );
};

export default LoadSampleButton;