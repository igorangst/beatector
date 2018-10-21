import threading
import queue
import logging

#class Sync:
queueEvent  = threading.Event() 
terminate   = threading.Event()

qLock       = threading.Lock()
queue       = queue.Queue(100)

def putEvent(t):
    qLock.acquire()
    queue.put(t)
    logging.debug("beat event @%i" % t)
    qLock.release()
    queueEvent.set()
