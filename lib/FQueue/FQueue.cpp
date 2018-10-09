#include <FQueue.h>

FQueue::FQueue(const int capacity, unsigned long *buf)
    : _buf(buf),
      _capacity(capacity),
      _wptr(0),
      _rptr(0),
      _size(0)
{
    
}

FQueue::~FQueue()
{

}

bool FQueue::full() const
{
    return _size == _capacity;
}

bool FQueue::empty() const
{
    return _size == 0;
}

int FQueue::size() const
{
    return _size;
}

int FQueue::capacity() const
{
    return _capacity;
}

void FQueue::push(const unsigned long element)
{
    _buf[_wptr] = element;
    _wptr = (_wptr + 1) % _capacity;
    if (full()){
        _rptr = (_rptr + 1) % _capacity;
    } else {
        ++_size;
    }
}

unsigned long FQueue::pop()
{
    if (empty()){
        return 0;
    }
    --_size;
    unsigned long front = _buf[_rptr];
    _rptr = (_rptr + 1) % _capacity;
    return front;
}

// returns oldest element
unsigned long FQueue::front() const
{
    if (empty()){
        return 0;
    }
    return _buf[_rptr];
}

// returns newest element
unsigned long FQueue::back() const
{
    if (empty()){
        return 0;
    }
    return _buf[(_wptr - 1) % _capacity];
}

unsigned long FQueue::at(const int idx) const
{
    if (idx < 0 || idx >= _size) {
        return 0;
    }
    return _buf[(_rptr + idx) % _capacity];
}

void FQueue::flush()
{
  _rptr = _wptr;
  _size = 0;
}
