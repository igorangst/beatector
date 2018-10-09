#include <Timer.h>
#include <FQueue.h>

const int metronomePin  = 15;  // Metronome LED
const int beatPin       = 3;   // Detected beat
const int statusPin     = 14;  // Stopped = on / armed = blinking / rolling = off
const int buttonPin     = 2;   // Arm / stop push button

// Global state machine
enum State {
	STOPPED,
	ARMED,
	ROLLING
};
State state;


// Beat predictor
typedef unsigned long time;
time          beats[16];
FQueue        beatQ(16, beats);
time          period;
Timer         timer;
bool          newEvents;

// metronome
bool   metronomeState = false;
time   metronomeTime  = 0;

// variables used for debouncing
time   beatChanged = 0;
time   buttonChanged = 0;
time   blink = 0;

// interrupt service routines
void detectBeat() {
	beatChanged = millis();
}

void detectButton() {
	buttonChanged = millis();
}

void setup() {
    // initialize serial communications at 9600 bps:
    Serial.begin(9600);

		state = STOPPED;
    period = 0;
    newEvents = false;

    pinMode(metronomePin, OUTPUT);
		pinMode(statusPin, OUTPUT);
    pinMode(beatPin, INPUT_PULLUP);
		pinMode(buttonPin, INPUT_PULLUP);

    digitalWrite(statusPin, HIGH);

    attachInterrupt(digitalPinToInterrupt(beatPin), detectBeat, CHANGE);
		attachInterrupt(digitalPinToInterrupt(buttonPin), detectButton, CHANGE);
}

void checkBeat(){
	if (beatChanged){

		// debounce
		time now = millis();
		if (now - beatChanged >= 5 && digitalRead(beatPin)){
			if (state != STOPPED) {
				beatQ.push(beatChanged);
				newEvents = true;
			}
			Serial.print("beat@");
			Serial.println(beatChanged);
      beatChanged = 0;
		}
	}
}

void checkButton(){
	if (buttonChanged){

		// debounce		
		time now = millis();
		if (now - buttonChanged >= 5 && digitalRead(buttonPin)){
			buttonChanged = 0;
			if (state == STOPPED) {
				state = ARMED;
				digitalWrite(statusPin, LOW);
				blink = millis();
			} else {
				state = STOPPED;
				digitalWrite(statusPin, HIGH);
			}
		}
	}
}


void loop() {
    timer.update();

		checkBeat();
		checkButton();

    if (state == ARMED && newEvents){
        if (beatQ.size() >= 4){
					  state = ROLLING;
						digitalWrite(statusPin, LOW);
            newEvents = false;
            period = predict();
            timer.after(period, schedule);
        }
    }

		time now = millis();
    if (metronomeState){
        if (now - metronomeTime > 40){
            digitalWrite(metronomePin, LOW);
        }
    }

		if (state == ARMED){
			if (now - blink >= 100) {
				digitalWrite(statusPin, LOW);
				blink = now;
			} else if (now - blink >= 50) {
				digitalWrite(statusPin, HIGH);
			}
		}
}

void schedule(){

  if (state == STOPPED){
    digitalWrite(metronomePin, LOW);
    beatQ.flush();
  } else {
    digitalWrite(metronomePin, HIGH);
    metronomeState = true;
    unsigned long now = millis();
    metronomeTime = now;

    if (newEvents){
      newEvents = false;
      period = predict();
    }
    Serial.print("[BEAT] period:");
    Serial.println(period);

    if (period == 0)
      return;

    time elapsed = now - beatQ.at(0);
    if (elapsed < period / 4)
      timer.after(period - elapsed, schedule);
    else
      timer.after(period, schedule);
  }
}

const time MAX_PERIOD = 60000 / 86;
const time MIN_PERIOD = 1 + MAX_PERIOD / 2;

time frame(time period){
    if (period == 0)
        return 0;
    while (period < MIN_PERIOD) period *= 2;
    while (period > MAX_PERIOD) period /= 2;
    if (period < MIN_PERIOD || period > MAX_PERIOD)
        return 0;
    return period;
}

long cmodsq(time a, time m){
  long r = a % m;
  if (r < (long)m/2)
    return sq(r);
  return sq(m - r);
}

const int QLEN = 8;
const int GUESSES = QLEN * (QLEN-1) / 2;
time periods[GUESSES];
long scores[GUESSES];

long predict(){
    time now = millis();
    int virtualPivot = 0;
    time pivot = 0;

    // If there hasn't been a beat close to now,
    // use the current time as pivot element
    time lastBeat = beatQ.back();
    if (period > 0 && (lastBeat < now - period / 4)){
        pivot = now;
        virtualPivot = 1;
    } else {
        pivot = lastBeat;
    }

    // shrink beat queue
    while (beatQ.size() + virtualPivot > QLEN){
        beatQ.pop();
    }

    // remove too old events (2 bars at curent bpm)
    if (period > 0 && now > 8*period){
        time horizon = now - 8*period;
        while (beatQ.front() < horizon)
		    beatQ.pop();
    }
    // Serial.println(beatQ.size());

    // keep current bpm if there are too few recent events
    int n = beatQ.size();
    if (n < 4) 
        return period;

    // collect inter-event differences as candidate periods
    int nPeriods = 0;
    if (virtualPivot){
        for (int i=0; i<n; ++i){
            time d = frame(pivot - beatQ.at(i));
            if (d > 0){
                periods[nPeriods++] = d;
            }
        }
    }
    for (int i=0; i<n-1; ++i){
        for (int j=i+1; j<n; ++j){
            time d = frame(beatQ.at(j) - beatQ.at(i));
            if (d > 0) {
                periods[nPeriods++] = d;
            }
        }
    }
    //Serial.print("#guesses = ");
    //Serial.println(nPeriods);

    // evaluate guessed periods
    for (int i=0; i<nPeriods; ++i){

        // half period grid
        time ht = periods[i] / 2;

        long e = 0;
        for (int j=0; j<beatQ.size() - 1 + virtualPivot; ++j){
            long d = pivot - beatQ.at(j);
            e += cmodsq(d, ht);
        }
        scores[i] = e;
        /* Serial.print("guess = "); */
        /* Serial.println(periods[i]); */
        /* Serial.print("score = "); */
        /* Serial.println(e); */
    }

    // find K best scores (or the best on the first run)
    int K = 3;
    if (period == 0)
      K = 1;
    unsigned long tScore;
    unsigned long tPeriod;
    for (int k=0; k<K; ++k){
        for (int i=nPeriods-2; i>=k; --i){
            if (scores[i] > scores[i+1]){
                tScore = scores[i];
                tPeriod = periods[i];
                scores[i] = scores[i+1];
                periods[i] = periods[i+1];
                scores[i+1] = tScore;
                periods[i+1] = tPeriod;
            }
        }
    }
    Serial.print("best = ");
    Serial.println(scores[0]);

    // choose the one closest to the current period
    unsigned long minDist = abs(periods[0] - period);
    unsigned long period = periods[0];
    for (int i=1; i<K; ++i){
      unsigned long dist = abs(periods[i] - period);
      if (dist < minDist){
        minDist = dist;
        period = periods[i];
      }
    }

    int bpm = 60000 / period;
    Serial.print(bpm);
    Serial.println(" bpm");

    return period;
}

