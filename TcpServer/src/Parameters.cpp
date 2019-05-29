#include "Parameters.h"

Parameters::Parameters() :
    _volume(0), _spdif_input(0), _auto_find(true)
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

void Parameters::setSpdifInput(int input)
{
    _spdif_input = input;
}

int Parameters::getSpdifInput()
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

Parameters* Parameters::instance()
{
    static Parameters inst;
    return &inst;
}
