#include "TCPConnection.h"
#include "TCPState.h"
#include "TCPClosed.h"
#include "TCPListen.h"
#include "ESP8266.h"

TCPConnection::TCPConnection() :
        _state(NULL),
        _esp(new ESP8266())
{

}

void TCPConnection::initialize()
{
    _esp->initialize();
}

void TCPConnection::handleMessage(message_t msg)
{
    switch (msg.event) {
    case EVENT_SERIAL_DATA_RECEIVED:
        if (_state != NULL) {
            _state->processLine(this);
        } else {
            _esp->processLine();
        }
        break;
    case EVENT_SERIAL_CMD_SEND:
        _esp->sendNextCommand();
        //_timeout.attach(callback(this, &EspInitState::sendNextCommand), 0.1);
        break;
    default:
        break;
    }
}

void TCPConnection::changeState(TCPState* state)
{
    _state = state;
    //_state->onStateEnter(this);
}

void TCPConnection::listen()
{
    changeState(TCPListen::instance());
    //_state->open();
}

void TCPConnection::close()
{
    //changeState(TCPClosed::instance());
    _state->close();
}

void TCPConnection::send(const char* buff)
{
    //_esp->printf(buff);
    _state->send(buff);
}

void TCPConnection::initLargeRxBuffer()
{
    _esp->initBuffers(ESP8266::LARGE_RX_BUF);
}

void TCPConnection::initLargeTxBuffer()
{
    _esp->initBuffers(ESP8266::LARGE_TX_BUF);
}

void TCPConnection::getRxBuffer(char** buff, int* len)
{
    _esp->getRxBuffer(buff, len);
}

void TCPConnection::getTxBuffer(char** buff, int* len)
{
    _esp->getTxBuffer(buff, len);
}

void TCPConnection::sendTxBuffer()
{
    _esp->sendTxBuffer();
}
