#include "ParametersEditor.h"

ParametersEditor::ParametersEditor(AudioProcessor* proc, ParametersManager& pmgr)
    : AudioProcessorEditor(proc), params(pmgr)
{
    params.createComponents(this);
    params.addListener(this);
    params.setDefaultParentSize(this);
}

ParametersEditor::~ParametersEditor()
{
    params.removeListener(this);
    params.deleteComponents(this);
}

void ParametersEditor::parameterValueChanged(int parameterIndex, float /*newValue*/)
{
    params.updateUIFromParameters(); 
}

void ParametersEditor::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void ParametersEditor::resized()
{
    params.setDefaultLayout(this);
}
