#include "MyRobot.h"

RobotDemo::RobotDemo(void):
	BackMotors(1, 3),
	FrontMotors(2, 4),
	sonicSensor(1),
	rightEncA(3),
	rightEncB(4),
	leftEncA(1),
	leftEncB(2),
	leftWheels(leftEncA, leftEncB, false, Encoder::k4X),
	rightWheels(rightEncA, rightEncB, false, Encoder::k4X),
	gamepad(1),
	comp(5,1),
	upLoader(1),
	downLoader(2),
	shooter(5),
	shooterLimit(7),
	isShootingManually(false),
	pi(14),
	running(false),
	readytoshoot(false),
	lastTime(0),
	lastDistance(0),
	speed(0),
	distanceBufferPosition(0)
{
	
	upLoader.Set(true);
	downLoader.Set(false);
	leftWheels.Start();
	rightWheels.Start();
	BackMotors.SetExpiration(0.1);
	FrontMotors.SetExpiration(0.1);
	for(int i = 0; i < DISTANCEBUFFERSIZE; i++)
	{
		distanceBuffer[(distanceBufferPosition % DISTANCEBUFFERSIZE)] = 0;
	}
}


double RobotDemo::GetBufferedDistance()
{
	double sum = 0;
	for(int i = 0; i < DISTANCEBUFFERSIZE; i++)
	{
		sum += distanceBuffer[i];
	}
	return sum / DISTANCEBUFFERSIZE;
}
	
	
	void RobotDemo::OperatorControl(void)
	{
		totalTimer.Reset();
		totalTimer.Start();
		comp.Start();
		double averageSpeed = 0;
		BackMotors.SetSafetyEnabled(true);
		FrontMotors.SetSafetyEnabled(true);
		shooter.SetSafetyEnabled(true);
		bool wasManualButtonPressed = false;
		bool wasSlowButtonPressed = false;
		bool wasReverseButtonPressed = false;
		bool wasShootButtonPressed = false;
		bool slowMode = false;
		bool reverseMode = true;
		while (IsOperatorControl() && IsEnabled())
		{
#if false
			//DISTANCEBUFFER UPDATE
			distanceBuffer[(distanceBufferPosition++ % DISTANCEBUFFERSIZE)] = GetDistanceInStupidInches(sonicSensor);
			
			
			//SPEED CALCULATION
			double _dist = GetBufferedDistance();
			double _time = totalTimer.Get();
			if((_time - lastTime) > 0.3){
				speed = (lastDistance - _dist) / (_time - lastTime);
				printf("%f = (%f - %f) / (%f - %f)\n", speed, _dist, lastDistance, _time, lastTime);
				lastTime = _time;
				lastDistance = _dist;
			}
#endif	
			
			
			
			if(!reverseMode)
			{
				leftStickSpeed = gamepad.GetRawAxis(4);
				rightStickSpeed = gamepad.GetRawAxis(2);
			}
			if (reverseMode)
			{
				leftStickSpeed = -gamepad.GetRawAxis(2);
				rightStickSpeed = -gamepad.GetRawAxis(4);
			}
			
			if (slowMode)
			{
				leftStickSpeed*=0.5;
				rightStickSpeed*=0.5;
			}
			averageSpeed = avg(leftStickSpeed,rightStickSpeed);
			
			
			
			//SMART DASHBOARD OUTPUT
			SmartDashboard::PutNumber("distance",GetDistanceInStupidInches(sonicSensor));
			SmartDashboard::PutBoolean("nonslowmode", !slowMode);
			SmartDashboard::PutBoolean("nonreversemode", !reverseMode);
			
			
			//SLOW MODE LOGIC
			if (gamepad.GetRawButton(3))
			{
				if(!wasSlowButtonPressed){
					slowMode=!slowMode;
				}
				wasSlowButtonPressed = true;
			}
			else
				wasSlowButtonPressed = false;
			
			//REVERSE MODE LOGIC
			if (gamepad.GetRawButton(2))
			{
				if(!wasReverseButtonPressed){
					reverseMode=!reverseMode;
				}
				wasReverseButtonPressed = true;
			}
			else
				wasReverseButtonPressed = false;
			
			
			//LIFTING
			
			if (gamepad.GetRawButton(5))
			{
				upLoader.Set(true);
				downLoader.Set(false);
				if(!running && !readytoshoot)
				{
					ShootSafe();
				}
			}
			if (gamepad.GetRawButton(7))
			{
				upLoader.Set(false);
				downLoader.Set(true);
			}
			
			
			// SHOOTING
			
			// This needs to be buffered so it doesn't continously trigger firing and loading in case the driver holds onto the button
			
			if (gamepad.GetRawButton(6)) //If bringDown or fire pressed, turn the kicker motor
			{
				if(!wasShootButtonPressed)
					ShootSafe();
				wasShootButtonPressed = true;
			}
			else if(gamepad.GetRawButton(8))
			{
				if(!wasShootButtonPressed)
					ShootOverride();
				wasShootButtonPressed = true;
			}
			else
				wasShootButtonPressed = false;
			
			
			//toggle manual mode
			if (gamepad.GetRawButton(9))
			{
				if(!wasManualButtonPressed)
				{
					isShootingManually = !isShootingManually;
				}
				wasManualButtonPressed = true;
			}
			else
				wasManualButtonPressed = false;
			//update shooter motors
			ShooterUpdate();
			
			//DRIVE CODE
			if (gamepad.GetRawAxis(6) == 1) //If the dpad arrow up is pushed, half power forwards
			{
				if(reverseMode)
				{
					BackMotors.TankDrive(-.5,-.5,0);
					FrontMotors.TankDrive(-.5,-.5,0);
				}
				else
				{
					BackMotors.TankDrive(.5,.5,0);
					FrontMotors.TankDrive(.5,.5,0);
				}
			}
			else if (gamepad.GetRawAxis(6) == -1) //If the dpad arrow down is pushed, half power backwards
			{
				if(reverseMode)
				{
					BackMotors.TankDrive(.5,.5,0);
					FrontMotors.TankDrive(.5,.5,0);
				}
				else
				{
					BackMotors.TankDrive(-.5,-.5,0);
					FrontMotors.TankDrive(-.5,-.5,0);
				}
			}
			else if (fabs(averageSpeed) <= 0.8) //Regular speed control if the average of both sticks is less than .8
			{
				BackMotors.TankDrive(leftStickSpeed, rightStickSpeed, 0);
				FrontMotors.TankDrive(leftStickSpeed, rightStickSpeed, 0);
				//printf("left %f rigth %f\n", leftStickSpeed, rightStickSpeed);
			}
			else //Average speed stabilizer if average of both sticks is greater than .8
			{
				BackMotors.TankDrive(averageSpeed, averageSpeed, 0);
				FrontMotors.TankDrive(averageSpeed, averageSpeed, 0);
			}
			
			//printf("%f - %f (%f)\n", speed, GetDistanceInStupidInches(sonicSensor), sonicSensor.GetVoltage());
			printf("%d\n", pi.Get());
			
			Wait(0.005);
		}
		
		comp.Stop();
		
	}
	
	
START_ROBOT_CLASS(RobotDemo);
