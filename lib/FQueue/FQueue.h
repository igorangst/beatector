#ifndef FQUEUE_H
#define FQUEUE_H

class FQueue {
public:
    FQueue(const int capacity, unsigned long *buf);
    ~FQueue();
    
    void push(const unsigned long element);
    unsigned long pop();
    unsigned long front() const;
    unsigned long back() const;
    unsigned long at(const int idx) const;
    bool full() const;
    bool empty() const;
    int size() const;
    int capacity() const;
    void flush();
    
private:
    unsigned long *_buf;
    int            _capacity;
    int            _wptr, _rptr;
    int            _size;
};

#endif

