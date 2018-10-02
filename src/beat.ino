#include <FQueue.h>

unsigned long event_buf[16];
unsigned long period = 0;

FQueue eventQ(16, event_buf);

void setup()
{
  
}

void loop()
{

}

unsigned long lastBeat()
{
	unsigned long now = millis();
	unsigned long horizon = now - period / 4;

	if (eventQ.at(0) >= horizon){
		return eventQ.at(0);
	} else {
		return now;
	}		
}

void estimate()
{
	unsigned long pivot = lastBeat();
	
}
