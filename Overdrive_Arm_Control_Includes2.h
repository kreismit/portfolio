//------------------------------------------------------------------------
// This task runs two PID controllers to position the two lower arms
// and the upper arm assembly
// Both setpoint and measurements are normalized to 0..100%
//------------------------------------------------------------------------

float SetPt;						// Setpoint for arm position

// Right Side Arm PID Values
int Ceiling = 2470;		// Right Side Full Up Value from Pot
int Floor = 700;				// Right Side Full Down Value from Pot
float Slope = 1;				// Right Side Slope of Pot
float CurrP;						// Current Position (0-100)
float PrevP;						// Previous Position

float CurrError = 0;		// Current Error
float PrevError = 0;		// Previous Error
float dP;								// Change in position, this iteration
float Proportional = 0;// Proportional Term of Left Side PID
float Int = 0;					// Integral Term of Left Side PID
float Deriv = 0;				// Derivative Term of Left Side PID
int Output = 0;						// Calculated Arm Motor Power

// Shared Values
float deltaT;							//Change in time (used for calculating dP/dt)
float Prop_Gain = 6;		// PID Tuning Constants
float Int_Gain = 0.8;
float Deriv_Gain = 0;

task armcontrol()
{
	//Initialize values
	Slope = (Ceiling - Floor)/100;		// dy/dx - normalized to 100
	Output = 0;
	deltaT = .005;
	clearTimer(T1);							// Reset Timer T1
	PrevP = (SensorValue[ArmPot] - Floor) / Slope;
	wait1Msec(5);

	do
	{
		deltaT = time1[T1]/1000;	// Time in seconds
		clearTimer(T1);						// Reset the timer T1

		// Scale current sensor values
		CurrP = (SensorValue[ArmPot] - Floor) / Slope;

		// Limit setpoint to 0..100

		if(SetPt < 0)
			SetPt = 0;
		if(SetPt > 100)
			SetPt = 100;

		// Define error values based on setpoint
		// Lower arm error uses average of two lower pots
		PrevError = SetPt - PrevP;
		CurrError = SetPt - CurrP;
		//if(abs(SetPt - CurrP) > 0.1)			//Deadband
		//	CurrError = SetPt - CurrP;
		//else
		//	CurrError = 0;

		// Update differentials
		dP = CurrError - PrevError;

		// Update P, I, and D
		// Proportional Term
		Proportional = Prop_Gain * CurrError;

		// Integral Term

		Int = Int + Int_Gain * CurrError * deltaT;

		// Derivative Term
		if(deltaT > 0)
		{
			Deriv = Deriv_Gain * (dP / deltaT);
		}

		// Update motor values

		if (Int > 100)
			Int = 100;	// Adjust the integral term to prevent integral windup
		if (Int < -100)
			Int = -100;

		Output = Proportional + Int + Deriv;

		//Limit motor values to -127 < PWM < 127
		if(Output < -127)
			Output = -127;
		if(Output > 127)
			Output = 127;

		// Update motor power
			motor[LRArm] = motor[LLArm] = motor[URArm] = motor[ULArm] = Output;

		//Update previous values
		PrevError = CurrError;
		wait1Msec(1);
	}
	while(true);
}
