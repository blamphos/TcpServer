#include "DebugPrint.h"

DebugPrint::DebugPrint()
{
    //ctor
}

DebugPrint::~DebugPrint()
{
    //dtor
}

DebugPrint* DebugPrint::instance()
{
    static DebugPrint inst;
    return &inst;
}
