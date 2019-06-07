#ifndef FIFO_H
#define FIFO_H

#include "mbed.h"

template<typename T>
class Fifo {
public:
	Fifo(size_t size) :
			_buf(new T[size]), _head(0), _tail(0), _size(size)
	{

	}

	void push(T item)
	{
		_buf[_head] = item;
		_head = (_head + 1) % _size;

		if (_head == _tail) {
			_tail = (_tail + 1) % _size;
		}
	}

	T pop()
	{
		if (empty()) {
			return T();
		}

		//Read data and advance the tail (we now have a free space)
		T item = _buf[_tail];
		_tail = (_tail + 1) % _size;

		return item;
	}

	void reset()
	{
		_head = _tail;
	}

	bool empty()
	{
		//if head and tail are equal, we are empty
		return _head == _tail;
	}

	bool full()
	{
		//If tail is ahead the head by 1, we are full
		return ((_head + 1) % _size) == _tail;
	}

	size_t size()
	{
		return _size - 1;
	}

private:
	T* _buf;
	size_t _head;
	size_t _tail;
	size_t _size;
};

#endif

