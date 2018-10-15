#!/usr/bin/env python3

import numpy

# Non-normalized Gauss around t with standard deviation s
def nng(t, s):
    return numpy.exp(-t**2 / 2*s**2)


class Beat:
    def __init__(self, bpm=None, period=None, phase=None):
        if not bpm is None:
            self.bpm = bpm
            self.period = 60.0 / bpm
        elif not period is None:
            self.period = period
            self.bpm = 60.0 / period
        else:
            self.period = 0.5
            self.bpm = 120.0

        if not phase is None:
            self.phase = phase
        else:
            self.phase = 0.0    

    # Get the half beat time (contained in [0, 7]) closest to a given
    # event
    def closestEigth(self, e):
            
        # Align event on bar
        e = (e - self.phase) % (4 * self.period)
        
        # Get closest half beat
        return int(numpy.round(e / (self.period / 2))) % 8


beat = Beat(bpm=120)

ks = [beat.closestEigth(x*0.01) for x in range(400)]

print (ks)


