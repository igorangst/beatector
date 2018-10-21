import alsaseq
import threading
import sync
import time
import logging

class alsaInput (threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)

    def run(self):
        print ("starting alsa listener")
        while not sync.terminate.isSet():
            if alsaseq.inputpending():
                now = time.time()
                event = alsaseq.input()
                evtype = event[0]
                pitch = event[7][1]
                if evtype == alsaseq.SND_SEQ_EVENT_NOTEON:
                    sync.putEvent(now)
                else:
                    time.sleep(0.001)
                    continue
            else:
                time.sleep(0.001)
                continue
        print ("stopping alsa listener")
