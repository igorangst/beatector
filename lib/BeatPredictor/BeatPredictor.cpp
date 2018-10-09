#include "BeatPredictor.h"

BeatPredictor::BeatPredictor(FQueue *beatQ, const int qlen)
    : _qlen(qlen),
      _beatQ(beatQ),
      _period(0)
{
    int nguesses = qlen * (qlen - 1) / 2;
    _guesses = new long[nguesses];
}

BeatPredictor::~BeatPredictor()
{
    delete _guesses;
}

BeatPredictor::time BeatPredictor::frame(BeatPredictor::time period)
{
    if (period == 0)
        return 0;
    while (period < MIN_PERIOD) period *= 2;
    while (period > MAX_PERIOD) period /= 2;
    if (period < MIN_PERIOD || period > MAX_PERIOD)
        return 0;
    return period;
}

long BeatPredictor::period() const
{
    return _period;
}

void BeatPredictor::mkDistances()
{
    _nguesses = 0;
    int n = _beatQ->size();
    for (int i=0; i<n-1; ++i){
	for (int j=i+1; j<n; ++j){
	    time d = frame(_beatQ->at(j) - _beatQ->at(i));
	    if (d > 0){
		_guesses[_nguesses++] = d;
	    }
	}
    }
}

bool BeatPredictor::predict()
{
    if (_beatQ->size() < 4){	
	return false;
    }

    while (_beatQ->size() > _qlen){
	_beatQ->pop();
    }
    mkDistances();
    _period = _guesses[0];
    
    return true;
}
