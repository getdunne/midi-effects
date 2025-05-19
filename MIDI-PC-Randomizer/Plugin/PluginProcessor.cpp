#include "PluginProcessor.h"
#include "ParametersEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new ParametersEditor(this, params);
}

// Constructor: we must give this at least an output bus, or we'll get zero-length buffers in processBlock()
PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties().withOutput("output", AudioChannelSet::stereo()))
    , params(this)
    , gen(rd())
{
    midiChParam = params.addInt("midiChannel", "MIDI Channel", 0, 16, 0);
    outMinParam = params.addInt("outMin", "Out Min", 0, 127, 0);
    outMaxParam = params.addInt("outMax", "Out Max", 0, 127, 127);

    outMinParam->addListener(this);
    outMaxParam->addListener(this);
}

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout& layout) const
{
    if (layout.inputBuses.size() > 0) return false;
    //if (layout.outputBuses.size() > 0) return false;
    return true;
}

void PluginProcessor::prepareToPlay(double, int)
{
    parameterValueChanged(1, 0.0f);
}

void PluginProcessor::parameterValueChanged(int parameterIndex, float newValue)
{
    dis = std::uniform_int_distribution<>(*outMinParam, *outMaxParam);
}

// Process one buffer ("block") of data
void PluginProcessor::processBlock(AudioBuffer<float>&, MidiBuffer& midiIn)
{
    int matchChannel = *midiChParam;

    MidiBuffer midiOut;
    for (auto& mm : midiIn)
    {
        auto msg = mm.getMessage();
        auto pos = mm.samplePosition;
        auto ch = msg.getChannel();

        if ((matchChannel != 0) && (matchChannel != ch)) continue;

        if (msg.isProgramChange())
        {
            int progNum = dis(gen);
            auto outMsg = MidiMessage::programChange(ch, progNum);
            midiOut.addEvent(outMsg, pos);
        }
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
