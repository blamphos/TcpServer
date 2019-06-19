#include "EventQueue.h"

EventQueue::EventQueue(int queue_size) : _fifo(new Fifo<message_t>(queue_size))
{

}

bool EventQueue::isEmpty()
{
    return _fifo->empty();
}

void EventQueue::post(EventIdT event, uint32_t data)
{
	message_t msg;
	msg.event = event;
	msg.data = data;
	_fifo->push(msg);
}

bool EventQueue::getNext(message_t& msg)
{
	if (!_fifo->empty()) {
		msg = _fifo->pop();
		return true;
	}

	return false;
}

EventQueue* EventQueue::instance()
{
	static EventQueue inst(EVENT_QUEUE_SIZE);
	return &inst;
}
