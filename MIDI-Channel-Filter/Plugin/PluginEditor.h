#pragma once
#include <JuceHeader.h>
#include "BasicKnob.h"
#include "LabeledKnob.h"

class PluginProcessor;
class ParametersManager;

class PluginEditor  : public AudioProcessorEditor
                    , public AudioProcessorParameter::Listener
{
public:
    PluginEditor(PluginProcessor&, ParametersManager&);
    ~PluginEditor() override;

    // Component
    void resized() override;
    void paint(Graphics& g) override;

    // AudioProcessorParameter::Listener
    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int, bool) override {}

protected:
    PluginProcessor& processor;
    ParametersManager& params;

    BasicLookAndFeel lookAndFeel;

    GroupComponent progBankGroup;
    BasicKnob channelKnob;
    LabeledKnob labeledChannelKnob;
    ToggleButton progToggle, bankMsbToggle, bankLsbToggle;

    GroupComponent blockChannelGroup;
    ToggleButton channelToggle[16];

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
