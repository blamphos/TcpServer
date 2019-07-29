#include "TCPState.h"
#include "TCPConnection.h"

TCPState::TCPState()
{

}

void TCPState::changeState(TCPConnection* t, TCPState* state)
{
    t->changeState(state);
}
