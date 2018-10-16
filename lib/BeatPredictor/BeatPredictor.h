#ifndef BEATPREDICTOR_H
#define BEATPREDICTOR_H

#include "FQueue.h"

class BeatPredictor {    
public:
    typedef unsigned long time;

    static const time MAX_PERIOD = 60000 / 86;
    static const time MIN_PERIOD = 1 + MAX_PERIOD / 2;
    
    BeatPredictor(FQueue *beatQ, const int qlen = 8);
    ~BeatPredictor();

    bool predict();
    long period() const;

private:
    int     _qlen;
    FQueue* _beatQ;
    long    _period;
    long*   _guesses;
    int     _nguesses;

    void mkDistances();
    time frame(time p);
};

#endif
