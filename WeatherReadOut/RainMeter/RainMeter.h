// EECS 473
// Chris Fulara and Nicholas Dedenbach
// SMRTHAUS
//
// Simple interface used to control the 4-state stepper motor that represents
// chance of rain on our WWF. Can be used to specify a range of motion.
//
// REQUIRED: The step size of the stepper motor being used and the number of steps on the motor

#ifndef RainMeter_h
#define RainMeter_h

class RainMeter
{
	// Needed for calculating number of steps needed to get to a position
	double stepSize;
	double degreesPerPercent;
	
	// Needed for calculating which combo of signals is needed next
	int totalSteps;
	int currentStep = 0;
	
	// The 4 pins being used for control
	int pins[4];
	
	public:
		RainMeter();
		RainMeter(double stepSize, double maxDegrees, int *statePins);
		void setChance(int newChance);
		void reset(bool clockwise = true);
	
	private:
		void step(int numSteps, bool clockwise = true);
};

#endif