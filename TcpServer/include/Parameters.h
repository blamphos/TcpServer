#ifndef PARAMETERS_H
#define PARAMETERS_H

class Parameters
{
public:
    void setVolume(int volume);
    int getVolume();
    void setSpdifInput(int input);
    int getSpdifInput();
    void setAutoFind(bool state);
    bool getAutoFind();
    static Parameters* instance();

private:
    Parameters();

    int _volume;
    int _spdif_input;
    bool _auto_find;
};

#endif
