#include "rainMeter.h"
#include "math.h"
#include "Arduino.h"


RainMeter::RainMeter() {}

RainMeter::RainMeter(double stepSize, double maxDegrees, int *statePins) {
	this->stepSize = stepSize;
	this->degreesPerPercent = maxDegrees/100.0;
	this->totalSteps = (int)(maxDegrees/stepSize);
	this->pins[0] = *statePins;
	this->pins[1] = *(statePins + 1);
	this->pins[2] = *(statePins + 2);
	this->pins[3] = *(statePins + 3);
	
	reset();
}

void RainMeter::setChance(int newChance) {
	bool clockwise;
	
	double diff = newChance - ((currentStep * stepSize)/degreesPerPercent);
	int steps = abs((int)(diff * 0.01 * totalSteps));
	if (diff < 0) {
		clockwise = true;
	}
	else {
		clockwise = false;
	}
	
	step(steps, clockwise);
}

// Resets the stepper motor to its resting position
// Stopped by a physical barrier on the WWF wall mount
void RainMeter::reset(bool clockwise) {
	// Turn it its max number of steps (plus a few to be on the safe side)
	this->currentStep = 48;
	step(totalSteps + 5, clockwise);
	this->currentStep = 0;
}

void RainMeter::step(int numSteps, bool clockwise) {
	for(int i = 0; i < numSteps; i++) {
		if (clockwise) {
			this->currentStep--;
		}
		else {
			this->currentStep++;
		}
		
		switch((this->currentStep) % 4) {
			
			case 0:
				digitalWrite(pins[0], HIGH);
				digitalWrite(pins[1], LOW);
				digitalWrite(pins[2], HIGH);
				digitalWrite(pins[3], LOW);
				delay(10);
				break;
			case 1:
				digitalWrite(pins[0], HIGH);
				digitalWrite(pins[1], LOW);
				digitalWrite(pins[2], LOW);
				digitalWrite(pins[3], HIGH);
				delay(10);
				break;
			case 2:
				digitalWrite(pins[0], LOW);
				digitalWrite(pins[1], HIGH);
				digitalWrite(pins[2], LOW);
				digitalWrite(pins[3], HIGH);
				delay(10);
				break;
			case 3:
				digitalWrite(pins[0], LOW);
				digitalWrite(pins[1], HIGH);
				digitalWrite(pins[2], HIGH);
				digitalWrite(pins[3], LOW);
				delay(10);
				break;
		}
	}
}
