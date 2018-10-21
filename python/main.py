#! /usr/bin/env python3

import alsain
import beatestimator
import alsaseq
import time
import sync

from alsain import alsaInput
from beatestimator import Beatector

# initialize ALSA
alsaseq.client( 'Beatector', 1, 1, False )

alsain = alsaInput()
estimator = Beatector()

estimator.start()
alsain.start()
time.sleep(1)

try:
    while True:
        time.sleep(1)
except:
    sync.terminate.set()
    sync.queueEvent.set()
    estimator.join()
    alsain.join()

