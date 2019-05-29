#ifndef PARAMETERS_H
#define PARAMETERS_H

namespace Spdif {
enum InputTypeT {
	Coax1 = 0,
	Coax2,
	Opt1,
	Auto
};
}

class Parameters
{
public:
    Parameters();
    int getVolume();
    Spdif::InputTypeT getSpdifInput();
    bool getAutoFind();
    void setAutoFind(bool state);

protected:
    void setVolume(int volume);
    void setSpdifInput(Spdif::InputTypeT input);

private:
    int _volume;
    Spdif::InputTypeT _spdif_input;
    bool _auto_find;
};

#endif
