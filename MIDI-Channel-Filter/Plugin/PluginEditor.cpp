#include "PluginEditor.h"
#include "PluginProcessor.h"

PluginEditor::PluginEditor(PluginProcessor& p, ParametersManager& pm)
    : AudioProcessorEditor(p)
    , processor(p), params(pm)
    , channelKnob(1.0f, 16.0f)
    , labeledChannelKnob("Channel", channelKnob)
{
    setLookAndFeel(&lookAndFeel);

    int channel = processor.midiChParam->get();
    progBankGroup.setText("Program/Bank Filter: Ch." + String(channel));
    addAndMakeVisible(progBankGroup);

    channelKnob.setRange(1.0, 16.0, 1.0);
    channelKnob.setValue(double(channel), dontSendNotification);
    channelKnob.onValueChange = [&] {
        float channel = float(channelKnob.getValue());
        processor.midiChParam->setValueNotifyingHost(processor.midiChParam->convertTo0to1(channel));
        };
    addAndMakeVisible(labeledChannelKnob);

    progToggle.setButtonText("Prog.Change");
    progToggle.setToggleState(processor.filterProgParam->get(), dontSendNotification);
    progToggle.onClick = [&] {
        processor.filterProgParam->setValueNotifyingHost(progToggle.getToggleState() ? 1.0f : 0.0f);
        };
    addAndMakeVisible(progToggle);

    bankMsbToggle.setButtonText("Bank MSB");
    bankMsbToggle.setToggleState(processor.filterBankMsbParam->get(), dontSendNotification);
    bankMsbToggle.onClick = [&] {
        processor.filterBankMsbParam->setValueNotifyingHost(bankMsbToggle.getToggleState() ? 1.0f : 0.0f);
        };
    addAndMakeVisible(bankMsbToggle);

    bankLsbToggle.setButtonText("Bank LSB");
    bankLsbToggle.setToggleState(processor.filterBankLsbParam->get(), dontSendNotification);
    bankLsbToggle.onClick = [&] {
        processor.filterBankLsbParam->setValueNotifyingHost(bankLsbToggle.getToggleState() ? 1.0f : 0.0f);
        };
    addAndMakeVisible(bankLsbToggle);

    blockChannelGroup.setText("MIDI channels to block entirely");
    addAndMakeVisible(blockChannelGroup);

    for (int i = 0; i < 16; i++)
    {
        channelToggle[i].setButtonText(String(1 + i));
        channelToggle[i].setToggleState(processor.blockChannelParam[i]->get(), dontSendNotification);
        channelToggle[i].onClick = [&, i] {
            processor.blockChannelParam[i]->setValueNotifyingHost(channelToggle[i].getToggleState() ? 1.0f : 0.0f);
            };
        addAndMakeVisible(channelToggle[i]);
    }

    setSize(260, 120 + 126 + 30);
    
    params.addListener(this);
}

PluginEditor::~PluginEditor()
{
    setLookAndFeel(nullptr);
    params.removeListener(this);
}

void PluginEditor::resized()
{
    auto area = getLocalBounds().reduced(10);

    progBankGroup.setBounds(area.removeFromTop(120));
    auto controlsArea = progBankGroup.getBounds().reduced(10);
    controlsArea.removeFromTop(10);
    labeledChannelKnob.setBounds(controlsArea.removeFromLeft(controlsArea.getHeight()));
    controlsArea.removeFromLeft(10);
    controlsArea.removeFromTop(4);
    progToggle.setBounds(controlsArea.removeFromTop(24));
    bankMsbToggle.setBounds(controlsArea.removeFromTop(24));
    bankLsbToggle.setBounds(controlsArea.removeFromTop(24));

    blockChannelGroup.setBounds(area.removeFromBottom(126));
    controlsArea = blockChannelGroup.getBounds().reduced(10);
    controlsArea.removeFromTop(10);
    auto col = controlsArea.removeFromLeft(56);
    for (auto i = 0; i < 4; i++) channelToggle[i].setBounds(col.removeFromTop(24));
    col = controlsArea.removeFromLeft(56);
    for (auto i = 4; i < 8; i++) channelToggle[i].setBounds(col.removeFromTop(24));
    col = controlsArea.removeFromLeft(56);
    for (auto i = 8; i < 12; i++) channelToggle[i].setBounds(col.removeFromTop(24));
    col = controlsArea.removeFromLeft(56);
    for (auto i = 12; i < 16; i++) channelToggle[i].setBounds(col.removeFromTop(24));
}

void PluginEditor::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void PluginEditor::parameterValueChanged(int /*parameterIndex*/, float /*newValue*/)
{
    // just be lazy and update all controls
    int channel = processor.midiChParam->get();
    progBankGroup.setText("Program/Bank Filter: Ch." + String(channel));
    channelKnob.setValue(double(channel), dontSendNotification);

    progToggle.setToggleState(processor.filterProgParam->get(), dontSendNotification);
    bankMsbToggle.setToggleState(processor.filterBankMsbParam->get(), dontSendNotification);
    bankLsbToggle.setToggleState(processor.filterBankLsbParam->get(), dontSendNotification);

    for (int i = 0; i < 16; i++)
        channelToggle[i].setToggleState(processor.blockChannelParam[i]->get(), dontSendNotification);
}
