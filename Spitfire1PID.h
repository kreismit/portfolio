float setpoint = 97;			//Desired motor speed, in rpm
float speed;				//Real motor speed, in rpm
float p = 0;			//Proportional
float pgain = 8;	//Proportional gain
float i = 0;			//Integral
float iadd;				//Number to add to integral
float igain = 0.00009;	//Integral gain
float d = 0;			//Derivative
float dgain = 150;	//Derivative gain
float deltat;			//Change in time
float error;				//Setpoint - input
float prev_error;		//Old error for derivative (delta error / delta time)
int output;				//Output motor power
bool shoot = false;				//Variable which turns on when a ball is fired
float value = 0;
float v0 = 0;	//Last value which was filtered

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
	while(true)
	{
		if(SensorValue[Trigger] < 4)	//Feed forward code activated when firing a ball
		{
			shoot = true;
			//SetMotor(Loader, 110, true);
			while(shoot == true)	//Feed forward: Override PID when returning to setpoint
			{
				speed = filter(abs(SmartMotorGetSpeed(Flywheel1)));	//Use Smart Motor Library to calculate speed
				error = setpoint - speed;
				if(error < 0.8)
					shoot = false;
				SetMotor(Flywheel1, 127, true);	//Run all flywheel motors at max power
				SetMotor(Flywheel2, 127, true);
				SetMotor(Flywheel3, 127, true);
				//SetMotor(Loader, 0, true);
				wait1Msec(10);	//Wait some time so as not to hog the CPU
				iadd = 0;
			}
		}
		speed = filter(abs(SmartMotorGetSpeed(Flywheel1)));	//Use Smart Motor Library to calculate speed
		clearTimer(T1);	//Reset timer each execution
		error = setpoint - speed;	//Calculate error
		//Proportional correction is less at low speeds since the motors have more torque
		if(setpoint < 90)
			p = (pgain - 2) * error;
		else
			p = pgain * error;	//Proportional calculation (gain * error)
		if(p > 100)	//Proportional limits
			p = 100;
		if(p < -50)
			p = -50;
		wait1Msec(10);		//Execute every 10 milliseconds
		deltat = time1[T1];	//This way, deltat should equal 10 msec
		if(abs(error) < 25){	//Turn off integral when far away from setpoint
			iadd = igain * error * deltat;	//Otherwise, calculate integral addition normally
		}
		else
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
		prev_error = error;	//Make old error previous; will overwrite next execution
		SetMotor(Flywheel1, output, true);	//Send the output to the motors
		SetMotor(Flywheel2, output, true);
		SetMotor(Flywheel3, output, true);
	}
}
