#pragma once
#include <JuceHeader.h>
#include "ParametersManager.h"

class ParametersEditor  : public AudioProcessorEditor
                        , public AudioProcessorParameter::Listener
{
public:
    ParametersEditor(AudioProcessor*, ParametersManager&);
    ~ParametersEditor() override;

    void paint(Graphics&) override;
    void resized() override;

    // AudioProcessorParameter::Listener
    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}

private:
    ParametersManager& params;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametersEditor)
};
