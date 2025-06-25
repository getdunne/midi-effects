#pragma once
#include <JuceHeader.h>
#include "ParametersManager.h"
#include "FunctionTable.h"
#include <random>

class PluginProcessor   : public AudioProcessor
                        , public ChangeBroadcaster
{
public:
    PluginProcessor();
    virtual ~PluginProcessor() = default;
protected:
    bool isBusesLayoutSupported(const BusesLayout&) const override;

    // How the plugin describes itself to the host
public:
    const String getName() const override { return "MIDI-Channel-Filter"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }
    bool hasEditor() const override { return true; }
    AudioProcessorEditor* createEditor() override;

    // Multiple simultaneously-loaded presets aka "programs" (not used)
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const String getProgramName(int) override { return {}; }
    void changeProgramName(int, const String&) override {}

    // Actual audio processing
    void prepareToPlay(double /*sampleRate*/, int /*maxSamplesPerBlock*/) override {};
    void releaseResources() override {}
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;
    void processBlockBypassed(AudioBuffer<float>&, MidiBuffer&) override {}

    // Enabling the host to save and restore state
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Automation parameters
    ParametersManager params;

protected:
    AudioParameterInt* midiChParam;
    AudioParameterBool* filterProgParam;
    AudioParameterBool* filterBankMsbParam;
    AudioParameterBool* filterBankLsbParam;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
