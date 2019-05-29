#ifndef SYSTEMCONTROL_H
#define SYSTEMCONTROL_H

#include "stdint.h"
#include "Parameters.h"

class SystemControl : public Parameters
{
public:
    void onVolumeChanged(uint32_t data);
    static SystemControl* instance();
    void onInputChanged(Spdif::InputTypeT input);

private:
    SystemControl();
};

#endif
