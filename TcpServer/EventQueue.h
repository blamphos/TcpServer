#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include "EmbeddedMessages.h"
#include "SafeQueue.h"

class EventQueue {
public:
	~EventQueue();
	void post(EventIdT event, uint32_t data = 0);
	bool wait(message_t& msg, int timeout_ms = 0);
	static EventQueue* instance();

private:
	EventQueue();
	SafeQueue<message_t> _queue;
};

#endif
