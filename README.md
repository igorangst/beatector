# BEATECTOR: Live beat detector with MIDI clock output

This purpose of this project is to build an easy to use box that turns
audio cues into a MIDI clock signal. This can be used in a musical
live setup, using e.g. the bass drum as audio source and feeding a
sequencer with the generated MIDI clock. In this way, the drummer is
free to play her own tempo, and the electronics are following. 

The platform used will be an Arduino Micro/Leonardo. The beat
detection part is mostly taken care of by an Analog
circuit. Schematics and PCB layout files will follow soon.

## I/O

The box has the following inputs and outputs:

* Power supply (5V micro USB)
* Audio trigger in (6.3 mm jack)
* Audio trigger in (XLR, optional)
* Foot switch in (6.3 mm jack, optional)
* Sensitivity potentiometer
* Lag/lead potentiometer
* Stop/arm push button
* Tap push button? (optional)
* MIDI output
* LEDs: beat detect, beat predict, Status: stopped/armed/rolling

## Resources

* [Arduino MIDI tutorial](https://www.arduino.cc/en/Tutorial/Midi)
* [Peak detector circuit](http://afrotechmods.com/tutorials/2011/11/28/peak-detector-circuit-tutorial/)

## Parts

* Arduino Micro/Leonardo
* Power supply (micro USB, 5V)
* 2x 6.3 mm jack socket
* MIDI socket (5 PIN DIN 180 degrees)
* 2x linear pots, 50k will do 
* Push button
* 3x LEDs 
 
