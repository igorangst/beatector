#!/usr/bin/env python3

import numpy as np
import plotly as py
import plotly.graph_objs as go

# Non-normalized Gauss around t with standard deviation s
def nng(t, s):
    return np.exp(-t**2 / 2*s**2)


class Beat:
    def __init__(self, bpm=None, period=None, phase=None):
        if not bpm is None:
            self.bpm = float(bpm)
            self.period = 60.0 / bpm
        elif not period is None:
            self.period = float(period)
            self.bpm = 60.0 / period
        else:
            self.period = 0.5
            self.bpm = 120.0

        if not phase is None:
            self.phase = float(phase)
        else:
            self.phase = 0.0    

    # Get the half beat time (contained in [0, 7]) closest to a given
    # event
    def closestEigth(self, e):
            
        # Align event on bar
        e = (e - self.phase) % (4 * self.period)
        
        # Get closest half beat
        return int(np.round(e / (self.period / 2))) % 8
    
    def precision(self, e, sigma):

        # Align event on bar
        e = (e - self.phase) % (4 * self.period)
        
        # Get closest half beat index
        k = np.round(e / (self.period / 2))

        # Closest reference event
        t = k * (self.period / 2)

        # Non-normalized Gauss
        p = np.exp(-(t-e)**2 / (2*sigma**2))

        return p
        

beat = Beat(bpm=120)

xs = np.arange(0, 2, 0.001)
ys = [beat.precision(x, 0.02) for x in xs]

trace = go.Scatter(
    x = xs,
    y = ys
)

data = [trace]
py.offline.plot(data, filename='basic-line')

