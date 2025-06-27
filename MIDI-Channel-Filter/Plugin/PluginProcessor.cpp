#include "PluginProcessor.h"
#include "PluginEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor(*this, params);
}

// Constructor: we must give this at least an output bus, or we'll get zero-length buffers in processBlock()
PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties().withOutput("output", AudioChannelSet::stereo()))
    , params(this)
{
    midiChParam = params.addInt("midiChannel", "MIDI Channel", 1, 16, 1);
    filterProgParam = params.addBool("filterProgChange", "Program Change", true);
    filterBankMsbParam = params.addBool("filterBankMSB", "Bank Select MSB", true);
    filterBankLsbParam = params.addBool("filterBankLSB", "Bank Select LSB", true);

    for (int i = 0; i < 16; i++)
        blockChannelParam[i] = params.addBool("blockChannel" + String(1 + i), "Block Channel " + String(1 + i), false);
}

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout& layout) const
{
    if (layout.inputBuses.size() > 0) return false;
    //if (layout.outputBuses.size() > 0) return false;
    return true;
}

// Process one buffer ("block") of data
void PluginProcessor::processBlock(AudioBuffer<float>&, MidiBuffer& midiIn)
{
    int matchChannel = midiChParam->get();
    MidiBuffer midiOut;

    for (auto mm : midiIn)
    {
        auto inMsg = mm.getMessage();
        auto pos = mm.samplePosition;
        auto ch = inMsg.getChannel();

        if (blockChannelParam[ch - 1]->get()) continue;

        bool channelMatched = (ch == matchChannel);
        bool passMsg = true;

        if ((filterProgParam->get() && inMsg.isProgramChange()) ||
            (filterBankMsbParam->get() && inMsg.isControllerOfType(0)) ||
            (filterBankLsbParam->get() && inMsg.isControllerOfType(32)))
            passMsg = channelMatched;

        if (passMsg) midiOut.addEvent(inMsg, pos);
    }
    midiIn.swapWith(midiOut);
}

// Called by the host when it needs to persist the current plugin state
void PluginProcessor::getStateInformation (MemoryBlock& destData)
{
    params.getStateInformation(destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    params.setStateInformation(data, sizeInBytes);
}
