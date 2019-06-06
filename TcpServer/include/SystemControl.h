#ifndef SYSTEMCONTROL_H
#define SYSTEMCONTROL_H

#include "Parameters.h"

class SystemControl
{
public:
    void onVolumeChanged(uint32_t data);
    static SystemControl* instance();
    void onInputChanged(Spdif::InputTypeT input);

private:
    SystemControl();

    Parameters* _parameters;
};

#endif
