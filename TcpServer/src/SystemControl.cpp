#include "SystemControl.h"

SystemControl::SystemControl()
{

}

void SystemControl::onVolumeChanged(uint32_t data)
{
    setVolume((int)data);
}

void SystemControl::onInputChanged(Spdif::InputTypeT input)
{
    setSpdifInput(input);
}

SystemControl* SystemControl::instance()
{
    static SystemControl inst;
    return &inst;
}
