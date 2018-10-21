import threading
import sync
import time

from beat import Beat
from threading import Timer

class Beatector (threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.events = []
        self.state = 'INIT'
        self.beat = None
        self.phase = 0.0
        self.count = 0

    def hit(self):
        print ("BEAT %d" % (self.count + 1))
        self.count = (self.count + 1) % 4
        self.schedule()

    def initializeBeat(self):
        pivot = self.events[0]
        estimate = 0.0
        for i in range(1, len(self.events)):
            estimate += self.events[i-1] - self.events[i]
        estimate /= len(self.events) - 1
        print (estimate)
        phase = (pivot + estimate) % (4 * estimate)
        self.beat = Beat(period = estimate, phase = phase)

    def schedule(self):
        if sync.terminate.isSet():
            return

        if self.state == 'INIT':
            self.initializeBeat()
            beater = Timer(self.beat.period, self.hit)
            beater.start()
            self.state = 'ROLLING'
            return

        phase_adjust = self.beat.phase - self.phase
        beater = Timer(self.beat.period + phase_adjust, self.hit)
        beater.start()
        self.phase = self.beat.phase

    # Main thread. Pops events from the sync queue and inserts them
    # into the local event queue. Beat estimation is started as soon
    # as five events are in the queue.
    def run(self):
        print ("starting beat estimator")
        while not sync.terminate.isSet():
            sync.queueEvent.wait()
            sync.queueEvent.clear()
            while not sync.queue.empty():
                e = sync.queue.get()
                self.events.insert(0, e)
                if self.state == 'INIT':
                    if len(self.events) == 4:
                        self.schedule()
                elif self.state == 'ROLLING':
                    self.beat.resyncOn(e)
                    self.beat.adjustTempo(self.events)
                    print ('Period: %5.4f\tPhase: %5.4f' % (self.beat.period, self.beat.phase))
        print ("stopping beat estimator")
