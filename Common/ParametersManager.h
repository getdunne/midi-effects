#pragma once
#include <JuceHeader.h>

class ParametersManager
{
public:
    ParametersManager(AudioProcessor* owner);

    AudioParameterFloat* addFloat(const String& idToUse, const String& name, float minValue, float maxValue, float defaultValue);
    AudioParameterInt* addInt(const String& idToUse, const String& name, int minValue, int maxValue, int defaultValue);
    AudioParameterBool* addBool(const String& idToUse, const String& name, bool defaultValue);
    AudioParameterChoice* AddChoice(const String& idToUse, const String& name, StringArray strings, int defaultIndex);

    void addListener(AudioProcessorParameter::Listener* listener);
    void removeListener(AudioProcessorParameter::Listener* listener);

    AudioProcessorParameter* getParameter(const String& name) const;
    AudioProcessorParameter* getParameter(const int index) const;

    XmlElement* getStateXml(const String tagName);
    void putStateXml(XmlElement*);

    // convenience methods, for plug-ins whose state is ONLY parameters
    void getStateInformation(MemoryBlock& destData);
    void setStateInformation(const void* data, int sizeInBytes);

    // convenience methods, to create and maintain a generic GUI
    void createComponents(Component* parent);
    void deleteComponents(Component* parent);
    void updateUIFromParameters();
    void setDefaultParentSize(Component* parent);
    void setDefaultLayout(Component* parent);

private:
    template<typename AudioParameterFloatOrInt>
    void createSliderComponent(Component* parent, AudioParameterFloatOrInt* parameter);
    void createComponent(Component* parent, AudioParameterBool* parameter);
    void createComponent(Component* parent, AudioParameterChoice* parameter);
    int getComponentDefaultHeight(const Component* component) const;

    void addListener(AudioProcessorParameter* parameter, AudioProcessorParameter::Listener* listener);
    void removeListener(AudioProcessorParameter* parameter, AudioProcessorParameter::Listener* listener);

    // The AudioProcessor owns the parameters, but the ParametersManager object keeps pointers to them
    AudioProcessor* m_owner;
    Array<AudioProcessorParameter*> m_parameters;

    // The ParametersManager owns the child Components created by createComponents().
    OwnedArray<Component> m_components;
};
