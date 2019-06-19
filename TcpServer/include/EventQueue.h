#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include "Fifo.h"
#include "EmbeddedMessages.h"

class EventQueue {
public:
	void post(EventIdT event, uint32_t data = 0);
	bool getNext(message_t& msg);
	bool isEmpty();
	static EventQueue* instance();

private:
	enum ConstantsT {
		EVENT_QUEUE_SIZE = 42
	};

	EventQueue(int queue_size);

	Fifo<message_t>* _fifo;
};

#endif
