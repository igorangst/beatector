#!/usr/bin/env python3

import numpy as np
import plotly as py
import plotly.graph_objs as go
import random

# Non-normalized Gauss around t with standard deviation s
def nng(t, s):
    return np.exp(-t**2 / (2*s**2))

# lookup table for NNG between 0 and 4*sigma
lu256 = [16384, 16382, 16376, 16366, 16352, 16334, 16312, 16286,
         16256, 16223, 16185, 16144, 16099, 16049, 15997, 15940, 15880, 15816,
         15749, 15678, 15603, 15525, 15444, 15359, 15272, 15180, 15086, 14989,
         14889, 14785, 14679, 14570, 14459, 14345, 14228, 14108, 13987, 13863,
         13736, 13608, 13477, 13345, 13210, 13074, 12936, 12796, 12654, 12512,
         12367, 12222, 12075, 11927, 11778, 11628, 11477, 11325, 11173, 11020,
         10866, 10712, 10558, 10403, 10248, 10093, 9937, 9782, 9627, 9472,
         9317, 9163, 9008, 8855, 8701, 8549, 8397, 8245, 8095, 7945, 7796,
         7648, 7501, 7355, 7210, 7066, 6924, 6783, 6642, 6504, 6366, 6230,
         6095, 5962, 5830, 5700, 5572, 5445, 5319, 5195, 5073, 4953, 4834,
         4716, 4601, 4487, 4375, 4265, 4157, 4050, 3945, 3842, 3741, 3641,
         3543, 3447, 3353, 3261, 3170, 3081, 2994, 2909, 2825, 2743, 2663,
         2584, 2508, 2433, 2359, 2287, 2217, 2149, 2082, 2017, 1953, 1891,
         1830, 1771, 1713, 1657, 1603, 1549, 1497, 1447, 1398, 1350, 1304,
         1258, 1214, 1172, 1130, 1090, 1051, 1013, 976, 941, 906, 872, 840,
         808, 778, 748, 720, 692, 665, 640, 615, 590, 567, 544, 523, 502, 481,
         462, 443, 424, 407, 390, 373, 358, 343, 328, 314, 300, 287, 275, 263,
         251, 240, 229, 219, 209, 200, 191, 182, 174, 166, 158, 151, 144, 137,
         130, 124, 118, 113, 107, 102, 97, 92, 88, 83, 79, 75, 71, 68, 64, 61,
         58, 55, 52, 50, 47, 45, 42, 40, 38, 36, 34, 32, 30, 29, 27, 26, 24,
         23, 22, 20, 19, 18, 17, 16, 15, 14, 14, 13, 12, 11, 11, 10, 10, 9, 8,
         8, 7, 7, 7, 6, 6]

# Approximated non-normalized Gauss
def anng(t, sigma):
    t = abs(t) / sigma
    idx = int(64 * t)
    if idx >= 256:
        return 0.0
    else:
        g = lu256[idx]
        return float(g) / 16384

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

        # Synchronization threshold
        self.th_sync = 0.78

        # Standard deviation for accuracy calculation
        self.sigma_sync = 0.03

        # Adjustment parameter for phase synchronization
        self.beta_sync = 0.9

        # Tempo adjustmet threshold
        self.th_tempo = 0.78

        # Standard deviation for tempo matching
        self.sigma_tempo = 0.015

        # Adjustment paramter for tempo changes
        self.alpha_tempo = 0.8

        # Headroom for accuracy, values above will lead to threshold adjustment
        self.headroom = 0.1

        # Adjustment exponent for narrowing/widening
        self.gamma = 0.85

    # Align event within bar
    def align(self, e):
        return (e - self.phase) % (4 * self.period)

    # Get the half beat time (contained in [0, 8]) closest to a given
    # event
    def closestEighth(self, e):

        # Align event on bar
        e = self.align(e)

        # Get closest half beat
        return int(np.round(e / (self.period / 2)))

    def accuracy(self, e, sigma, approx=False):

        # Align event on bar
        e = self.align(e)

        # Get closest half beat index
        k = np.round(e / (self.period / 2))

        # Closest reference event
        t = k * (self.period / 2)

        # Non-normalized Gauss
        if approx:
            a = anng(t-e, sigma)
        else:
            a = nng(t-e, sigma)

        return a

    def weight(self, e, sigma, approx=False):
        a = self.accuracy(e, sigma, approx)
        k = self.closestEighth(e)

        def importance(k):
            return 0.4 if k % 2 else 1.0

        return (a * importance(k), k)

    def resyncOn(self, e):
        w, k = self.weight(e, self.sigma_sync, approx=True)

        print ('PHASE %5.4f (th: %5.4f, sigma: %5.4f)' % (w, self.th_sync, self.sigma_sync))

        if w >= self.th_sync:
            e_aligned = self.align(e)
            t_close = k * self.period / 2.0
            delta = t_close - e_aligned
            l_sync = 0.4 if k%2 else 1.0
            delta_t = (w + self.beta_sync) / (1 + self.beta_sync) * l_sync * delta
            self.phase += delta_t

            if w >= self.th_sync + self.headroom:
                self.th_sync = pow(self.th_sync, self.gamma)
                self.sigma_sync = self.sigma_sync * self.gamma
        else:
            self.th_sync = pow(self.th_sync, 1.0/self.gamma)
            self.sigma_sync = self.sigma_sync / self.gamma


    # Update tempo estimate based on a list of events (newest event is first)
    def adjustTempo(self, events):

        # Keep only events within last two measures
        horizon = events[0] - 8*self.period
        while events and events[-1] < horizon:
            events.pop()

        # align events and calculate nearest eighths
        pivot = events[0]
        def quantize(d):
            return int(np.round(2*d / self.period))
        ks = [(pivot - e, quantize(pivot - e)) for e in events]

        # weight function for quantized intervals
        def interval_weight(k):
            if k == 1:
                return 0.9
            elif k == 2:
                return 1.0
            elif k == 4:
                return 1.0
            elif k == 8:
                return 0.8
            elif k == 16:
                return 0.8
            else:
                return 0.0

        # calculate errors wrt predicted positions
        def error(d, k):
            return d - 0.5 * k * self.period
        errors = [(error(d, k), k) for (d,k) in ks]

        def score(e, k):
            return anng(e, self.sigma_tempo) * interval_weight(k)
        scores = [score(e,k) for (e,k) in errors]

        best_idx = np.argmax(scores)
        best_score = scores[best_idx]

        print ('TEMPO %5.4f (th: %5.4f, sigma: %5.4f)' % (best_score, self.th_tempo, self.sigma_tempo))

        if best_score >= self.th_tempo:
            best_error, best_k = errors[best_idx]
            adjust = - self.alpha_tempo * best_score * best_error / best_k
            print ('err: %5.4f, adj: %5.4f' % (best_error, adjust))
            self.period += adjust
            if best_score >= self.th_tempo + self.headroom:
                self.th_tempo = pow(self.th_tempo, self.gamma)
                self.sigma_tempo = self.sigma_tempo * self.gamma
        else:
            self.th_tempo = pow(self.th_tempo, 1.0/self.gamma)
            self.sigma_tempo = self.sigma_tempo / self.gamma
        return scores


if __name__ == '__main__':
    beat = Beat(bpm=120)

    q = list()
    now = 100.0
    q.append(now)
    for i in range(32):
        q.append(now - 0.25*(i+1) + 0.002 * random.randint(-10, 10))

    scores = beat.adjustTempo(q)
    pivot = q[0]
    xs = [pivot - e for e in q]

    strace = go.Scatter(
        x = xs,
        y = scores,
        mode = 'markers',
        marker = dict(size=8)
    )

    xs = np.arange(0, 4, 0.001)
    ys = [beat.weight(x, 0.03)[0] for x in xs]
    zs = [beat.weight(x, 0.03, approx=True)[0] for x in xs]

    ds = [beat.resyncOn(x) for x in xs]

    beat.beta_sync = 0.5
    es = [beat.resyncOn(x) for x in xs]

    trace = go.Scatter(
        x = xs,
        y = ys
    )

    atrace = go.Scatter(
        x = xs,
        y = zs
    )

    dtrace = go.Scatter(
        x = xs,
        y = ds
    )

    etrace = go.Scatter(
        x = xs,
        y = es
    )

    data = [trace, strace]
    py.offline.plot(data, filename='weight.html')

