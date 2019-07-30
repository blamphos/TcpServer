#include "TCPState.h"
#include "TCPConnection.h"

TCPState::TCPState()
{

}

void TCPState::open()
{

}

void TCPState::close()
{

}

void TCPState::send(const char* buff)
{

}

void TCPState::changeState(TCPConnection* t, TCPState* state)
{
    t->changeState(state);
}
