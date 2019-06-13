#ifndef DEBUGPRINT_H
#define DEBUGPRINT_H

class DebugPrint {
public:
    ~DebugPrint();
    static DebugPrint* instance();

private:
    DebugPrint();
};

#endif
