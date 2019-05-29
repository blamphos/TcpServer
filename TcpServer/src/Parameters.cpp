#include "Parameters.h"

Parameters::Parameters() :
    _volume(0), _spdif_input(Spdif::Coax1), _auto_find(true)
{

}

void Parameters::setVolume(int volume)
{
    _volume = volume;
}

int Parameters::getVolume()
{
    return _volume;
}

void Parameters::setSpdifInput(Spdif::InputTypeT input)
{
    _spdif_input = input;
}

Spdif::InputTypeT Parameters::getSpdifInput()
{
    return _spdif_input;
}

void Parameters::setAutoFind(bool state)
{
    _auto_find = state;
}

bool Parameters::getAutoFind()
{
    return _auto_find;
}
