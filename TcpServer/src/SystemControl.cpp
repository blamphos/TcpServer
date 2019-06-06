#include "SystemControl.h"

SystemControl::SystemControl()
{
    _parameters = Parameters::instance();
}

void SystemControl::onVolumeChanged(uint32_t data)
{
    _parameters->current_level = (int)data;
}

void SystemControl::onInputChanged(Spdif::InputTypeT input)
{
    _parameters->current_input = input;
}

SystemControl* SystemControl::instance()
{
    static SystemControl inst;
    return &inst;
}
