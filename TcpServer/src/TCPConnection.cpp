#include "TCPConnection.h"
#include "TCPState.h"
#include "TCPClosed.h"
#include "ESP8266.h"

TCPConnection::TCPConnection() :
        _state(TCPClosed::instance()),
        _esp(new ESP8266)
{

}

void TCPConnection::initialize()
{
    changeState(TCPClosed::instance());
    _esp->initialize();
}

void TCPConnection::handleMessage(message_t msg)
{
    switch (msg.event) {
    case EVENT_SERIAL_DATA_RECEIVED:
        //processLine(t);
        break;
    case EVENT_SERIAL_CMD_SEND:
        _esp->sendNextCommand();
        //_timeout.attach(callback(this, &EspInitState::sendNextCommand), 0.1);
        break;
    default:
        _state->handleMessage(this, msg);
        break;
    }
}

void TCPConnection::changeState(TCPState* state)
{
    _state->onStateExit(this);
    _state = state;
    _state->onStateEnter(this);
}

void TCPConnection::send(const char* buff)
{
    _esp->printf(buff);
}
