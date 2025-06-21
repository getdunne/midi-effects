#include "ParametersManager.h"

constexpr int DEFAULT_MARGIN = 20;
constexpr int DEFAULT_PADDING = 2;
constexpr int DEFAULT_SLIDER_WIDTH = 300;
constexpr int DEFAULT_LABEL_WIDTH = 90;
constexpr int DEFAULT_TEXT_BOX_WIDTH = 80;
constexpr int DEFAULT_SLIDER_HEIGHT = (20 + 2 * DEFAULT_PADDING);
constexpr int DEFAULT_TOGGLE_HEIGHT = (24 + 2 * DEFAULT_PADDING);
constexpr int DEFAULT_COMBO_HEIGHT = (24 + 2 * DEFAULT_PADDING);

#if JUCE_MAJOR_VERSION == 5
#define FOR_XML_BY_NAME(childXml, rootXml, quoted_name)\
for (auto childXml = rootXml->getChildByName(quoted_name); childXml != nullptr; childXml = childXml->getNextElementWithTagName(quoted_name))
#else
#define FOR_XML_BY_NAME(childXml, rootXml, quoted_name)\
for (auto childXml : rootXml->getChildWithTagNameIterator(quoted_name))
#endif


ParametersManager::ParametersManager(AudioProcessor * owner)
    : m_owner(owner)
{
    m_components.clear();
}

AudioParameterFloat* ParametersManager::addFloat(const String& idToUse, const String& name, float minValue, float maxValue, float defaultValue)
{
    auto param = new AudioParameterFloat(idToUse, name, minValue, maxValue, defaultValue);
    m_parameters.add(param);
    m_owner->addParameter(param);
    return param;
}

AudioParameterInt* ParametersManager::addInt(const String& idToUse, const String& name, int minValue, int maxValue, int defaultValue)
{
    auto param = new AudioParameterInt(idToUse, name, minValue, maxValue, defaultValue);
    m_parameters.add(param);
    m_owner->addParameter(param);
    return param;
}

AudioParameterBool* ParametersManager::addBool(const String& idToUse, const String& name, bool defaultValue)
{
    auto param = new AudioParameterBool(idToUse, name, defaultValue);
    m_parameters.add(param);
    m_owner->addParameter(param);
    return param;
}

AudioParameterChoice* ParametersManager::AddChoice(const String& idToUse, const String& name, StringArray strings, int defaultIndex)
{
    auto param = new AudioParameterChoice(idToUse, name, strings, defaultIndex);
    m_parameters.add(param);
    m_owner->addParameter(param);
    return param;
}

void ParametersManager::addListener(AudioProcessorParameter::Listener* listener)
{
    for (auto parameter : m_parameters)
    {
        parameter->addListener(listener);
    }
}

void ParametersManager::removeListener(AudioProcessorParameter::Listener* listener)
{
    for (auto parameter : m_parameters)
    {
        parameter->removeListener(listener);
    }
}

void ParametersManager::createComponents(Component* parent)
{
    deleteComponents(parent);
    for (auto parameter : m_parameters)
    {
        createSliderComponent<AudioParameterFloat>(parent, dynamic_cast<AudioParameterFloat*>(parameter));
        createSliderComponent<AudioParameterInt>(parent, dynamic_cast<AudioParameterInt*>(parameter));
        createComponent(parent, dynamic_cast<AudioParameterBool*>(parameter));
        createComponent(parent, dynamic_cast<AudioParameterChoice*>(parameter));
    }
}

void ParametersManager::deleteComponents(Component* parent)
{
    Array<Component*> componentsToRemove;

    for (auto component : m_components)
    {
        if (component->getParentComponent() == parent)
        {
            componentsToRemove.add(component);
        }
    }

    for (auto component : componentsToRemove)
    {
        m_components.removeObject(component);
    }
}

AudioProcessorParameter* ParametersManager::getParameter(const String& name) const
{
    for (auto parameter : m_parameters)
    {
        if (parameter->getName(256) == name)
        {
            return parameter;
        }
    }
    return nullptr;
}

AudioProcessorParameter* ParametersManager::getParameter(const int index) const
{
    AudioProcessorParameter* parameter = nullptr;
    if (isPositiveAndBelow(index, m_parameters.size()))
        parameter = m_parameters.getUnchecked(index);
    return parameter;
}

void ParametersManager::updateUIFromParameters()
{
    for (auto parameter : m_parameters)
    {
        String name = parameter->getName(256);

        // Find the parameter in the UI 
        // Can be improved with using a map
        for (auto component : m_components)
        {
            if (component->getName() == name)
            {
                auto* paramFloat = dynamic_cast<AudioParameterFloat*>(parameter);
                auto* paramInt = dynamic_cast<AudioParameterInt*>(parameter);
                auto* paramBool = dynamic_cast<AudioParameterBool*>(parameter);
                auto* paramChoice = dynamic_cast<AudioParameterChoice*>(parameter);

                auto* slider = dynamic_cast<Slider*>(component);
                auto* toggle = dynamic_cast<ToggleButton*>(component);
                auto* combobox = dynamic_cast<ComboBox*>(component);

                if (nullptr != slider && nullptr != paramFloat)
                {
                    slider->setValue(double(paramFloat->get()), dontSendNotification);
                }
                else if (nullptr != slider && nullptr != paramInt)
                {
                    slider->setValue(double(paramInt->get()), dontSendNotification);
                }
                else if (nullptr != toggle && nullptr != paramBool)
                {
                    toggle->setToggleState(paramBool->get(), dontSendNotification);
                }
                else if (nullptr != combobox && nullptr != paramChoice)
                {
                    combobox->setSelectedItemIndex(paramChoice->getIndex(), dontSendNotification);
                }
            }
        }
    }
}

XmlElement* ParametersManager::getStateXml(const String tagName)
{
    XmlElement* stateXml = new XmlElement(tagName);
    XmlElement* paramXml;

    for (auto parameter : m_parameters)
    {
        paramXml = new XmlElement("Param");
        paramXml->setAttribute("name", parameter->getName(256));

        if (auto paramBool = dynamic_cast<AudioParameterBool*>(parameter); nullptr != paramBool)
        {
            paramXml->setAttribute("value", paramBool->get());
        }
        else if (auto paramFloat = dynamic_cast<AudioParameterFloat*>(parameter); nullptr != paramFloat)
        {
            paramXml->setAttribute("value", paramFloat->get());
        }
        else if (auto paramInt = dynamic_cast<AudioParameterInt*>(parameter); nullptr != paramInt)
        {
            paramXml->setAttribute("value", paramInt->get());
        }
        else if (auto paramChoice = dynamic_cast<AudioParameterChoice*>(parameter); nullptr != paramChoice)
        {
            paramXml->setAttribute("value", paramChoice->getIndex());
        }

        stateXml->addChildElement(paramXml);
    }
    return stateXml;
}

void ParametersManager::putStateXml(XmlElement* stateXml)
{
    FOR_XML_BY_NAME(paramXml, stateXml, "Param")
    {
        String paramRef = paramXml->getStringAttribute("name");
        float paramValue = float(paramXml->getDoubleAttribute("value"));

        RangedAudioParameter* parameter = dynamic_cast<RangedAudioParameter*>(getParameter(paramRef));
        if (nullptr != parameter)
        {
            parameter->setValueNotifyingHost(parameter->convertTo0to1(paramValue));
        }
    }
}

void ParametersManager::getStateInformation(MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> stateXml(getStateXml("Parameters"));
    AudioProcessor::copyXmlToBinary(*stateXml, destData);
}

void ParametersManager::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> stateXml(AudioProcessor::getXmlFromBinary(data, sizeInBytes));
    jassert(stateXml);
    putStateXml(stateXml.get());
}

template<typename AudioParameterFloatOrInt>
void ParametersManager::createSliderComponent(Component* parent, AudioParameterFloatOrInt* parameter)
{
    if (nullptr != parameter)
    {
        static_assert(
            std::is_same_v<AudioParameterFloatOrInt, AudioParameterFloat> || std::is_same_v<AudioParameterFloatOrInt, AudioParameterInt>,
            "This function supports only AudioParameterFloat and AudioParameterInt");

        String name = parameter->name;

        // Create the component
        Slider* slider = new Slider(name);
        slider->setTextBoxStyle(Slider::TextBoxRight, false, DEFAULT_TEXT_BOX_WIDTH, DEFAULT_SLIDER_HEIGHT);
        slider->setRange(
            double(parameter->getNormalisableRange().getRange().getStart()),
            double(parameter->getNormalisableRange().getRange().getEnd()));
        slider->setDoubleClickReturnValue(true, 0.0);
        slider->setValue(double(parameter->get()), dontSendNotification);
        slider->onValueChange = 
            [parameter, slider] ()
            { 
                parameter->setValueNotifyingHost(parameter->convertTo0to1(float(slider->getValue()))); 
            };

        // Create the label
        Label* label = new Label(name);
        label->setText(name, dontSendNotification);
        label->attachToComponent(slider, true);

        // Maintain the new controls
        parent->addAndMakeVisible(slider);
        parent->addAndMakeVisible(label);

        m_components.add(slider);
        m_components.add(label);
    }
}

void ParametersManager::createComponent(Component* parent, AudioParameterBool* parameter)
{
    if (nullptr != parameter)
    {
        String name = parameter->name;

        ToggleButton* toggle = new ToggleButton(name);
        toggle->setButtonText(name);
        toggle->setToggleState(parameter->get(), dontSendNotification);
        
        toggle->onClick = 
            [parameter, toggle] ()
            { 
                parameter->setValueNotifyingHost(toggle->getToggleState() ? 1.0f : 0.0f); 
            };
        
        parent->addAndMakeVisible(toggle);
        m_components.add(toggle);
    }
}

void ParametersManager::createComponent(Component* parent, AudioParameterChoice* parameter)
{
    if (nullptr != parameter)
    {
        String name = parameter->name;

        ComboBox* combobox = new ComboBox(name);
        combobox->addItemList(parameter->choices, 1);
        combobox->setSelectedItemIndex(parameter->getIndex(), dontSendNotification);
        combobox->onChange = 
            [parameter, combobox] ()
            { 
                parameter->setValueNotifyingHost(parameter->convertTo0to1(float(combobox->getSelectedItemIndex()))); 
            };

        Label* label = new Label(name);
        label->setText(name, dontSendNotification);
        label->attachToComponent(combobox, true);

        parent->addAndMakeVisible(combobox);
        parent->addAndMakeVisible(label);
        m_components.add(combobox);
        m_components.add(label);
    }
}

void ParametersManager::addListener(AudioProcessorParameter* parameter, AudioProcessorParameter::Listener* listener)
{
    if (nullptr != parameter)
    {
        parameter->addListener(listener);
    }
}

void ParametersManager::removeListener(AudioProcessorParameter* parameter, AudioProcessorParameter::Listener* listener)
{
    if (nullptr != parameter)
    {
        parameter->removeListener(listener);
    }
}

int ParametersManager::getComponentDefaultHeight(const Component* component) const
{
    int height = 0;

    if (auto slider = dynamic_cast<const Slider*>(component))
    {
        height = DEFAULT_SLIDER_HEIGHT;
    }
    else if (auto toggle = dynamic_cast<const ToggleButton*>(component))
    {
        height = DEFAULT_TOGGLE_HEIGHT;
    }
    else if (auto combobox = dynamic_cast<const ComboBox*>(component))
    {
        height = DEFAULT_COMBO_HEIGHT;
    }
    
    return height;
}

void ParametersManager::setDefaultParentSize(Component* parent)
{
    int height = 2 * DEFAULT_MARGIN;
    constexpr int WIDTH = 2 * DEFAULT_MARGIN + DEFAULT_LABEL_WIDTH + DEFAULT_SLIDER_WIDTH;

    for (auto component : m_components)
    {
        height += getComponentDefaultHeight(component);
    }
    
    parent->setSize(WIDTH, height);
}

void ParametersManager::setDefaultLayout(Component* parent)
{
    auto area = parent->getLocalBounds().reduced(DEFAULT_MARGIN);
    area.removeFromLeft(DEFAULT_LABEL_WIDTH);

    for (auto component : m_components)
    {
        int height = getComponentDefaultHeight(component);

        if (height > 0)
        {
            component->setBounds(area.removeFromTop(height).reduced(0, DEFAULT_PADDING));
        }
    }
}
