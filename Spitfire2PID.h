float setpoint = 1890;			//Desired motor speed, in rpm
float rpm;					//Calculated speed, in rpm (unfiltered)
float speed;				//Real motor speed, in rpm
float p = 0;			//Proportional
float pgain = 0.5;	//Proportional gain
float i = 35;			//Integral
float iadd;				//Number to add to integral
float igain = 0.05;	//Integral gain
float d = 0;			//Derivative
float dgain = 0.0;	//Derivative gain
float deltat = 0.050;			//Change in time (1/20 second)
float error;				//Setpoint - input
float prev_error;		//Old error for derivative (delta error / delta time)
int output;				//Output motor power
bool BallDetected = false; //Variable which turns on when a ball is in the flywheel
bool shoot = false;				//Variable which turns on when a ball is fired
float value = 0;
float v0 = 0;	//Last value which was filtered
bool IntegralLock = false; //Variable which switches on to prevent integral from winding up

//The VEX encoders are flaky, so the bot has accuracy issues with an unfiltered PID.
//Feed this function a number, and it filters it based on the last value of that number.
float filter(float number){
	//Divide the difference between the old value and the new value by a set gain
	value = v0 + ((number - v0)/3);
	v0 = number;	//Update the old value
	return value;
}


task FlywheelRun()
{
	SensorValue[FWEncoder] = 0;	//Reset encoder to make sure there is no spike when it passes 0
	while(true)
	{
		if(SensorValue[LS] > 220)	//Detect when a ball is fed through the flywheel
			BallDetected = true;		//Set this boolean to true
		else
			BallDetected = false;
		if(BallDetected == true)	//Feed forward code activated when firing a ball
		{
			shoot = true;
			while(shoot == true)	//Feed forward: Override PID when returning to setpoint
			{
				//Reset Encoder for speed calculation
				SensorValue[FWEncoder] = 0;
				wait1Msec(50);	//Wait some time to calculate speed
				//Calculate velocity of encoder
				rpm = (SensorValue[FWEncoder])/0.3;
				//Translate this to the speed of the flywheel
				speed = abs(filter(rpm * 5));
				error = setpoint - speed;	//Calculate error
				if(error < 20)						//Stop the loop if error is within a certain bound
					shoot = false;
				motor[Flywheel1] = 127;	//Run all flywheel motors at max power
				motor[Flywheel2] = 127;
				motor[Flywheel3] = 127;
				writeDebugStreamLine("%4.2f, %2.2f, %2.2f, %2.2f, %i", speed, p, i, d, setpoint);
				iadd = 0;	//Leave integral the same when running feed-forward
				SensorValue[FWEncoder] = 0;
			}
			wait1Msec(50);
		}
		//Calculate velocity of encoder
		rpm = (SensorValue[FWEncoder])/0.3;
		//Translate this to the speed of the flywheel
		speed = abs(filter(rpm * 5));
		writeDebugStreamLine("%4.2f, %2.2f, %2.2f, %2.2f, %i", speed, p, i, d, setpoint);
		error = setpoint - speed;	//Calculate error
		p = pgain * error;	//Proportional calculation (gain * error)
		if(p > 100)	//Proportional limits
			p = 100;
		if(p < -50)
			p = -50;
		if(abs(error) < 500 && IntegralLock == false){
			iadd = igain * error * deltat;	//Otherwise, calculate integral addition normally
		}
		else  	//Turn off integral when far away from setpoint or integral lock is on
			iadd = 0;
		i = i + iadd;	//Sum integral
		if(i > 127)		//Integral ceiling
			i = 127;
		if(i < 0)	//Integral floor
			i = 0;
		//Calculate derivative as change in error divided by change in time
		d = dgain * (error - prev_error) / deltat;
		output = p + i + d;
		if(output < 0)	//Output floor (never want to reverse the motors)
			output = 0;
		if(output > 127)	//Output ceiling (can't go above 127)
			output = 127;
		//Send the output to the motors
		motor[Flywheel1] = motor[Flywheel2] = motor[Flywheel3] = output;
		//Reset Encoder for speed calculation
		SensorValue[FWEncoder] = 0;
		wait1Msec(50);	//Wait some time to calculate speed
		prev_error = error;	//Make old error previous; will overwrite next execution
	}
}
