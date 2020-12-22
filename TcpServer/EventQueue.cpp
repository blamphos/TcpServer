#include "EventQueue.h"

EventQueue::EventQueue()
{

}

EventQueue::~EventQueue()
{
	
}

void EventQueue::post(EventIdT event, uint32_t data)
{
	message_t msg;
	msg.event = event;
	msg.data = data;
	_queue.enqueue(msg);
}

bool EventQueue::wait(message_t& msg, int timeout_ms)
{
	bool result = true;

	if (timeout_ms == 0) {
		msg = _queue.dequeue();
	}
	else {
		result = _queue.dequeue(msg, timeout_ms);
	}
	return result;
}

EventQueue* EventQueue::instance()
{
	static EventQueue inst;
	return &inst;
}
