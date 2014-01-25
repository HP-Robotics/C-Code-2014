#include "WPILib.h"

#define NULLZONE .08f

//Marcell is awesome

class RobotDemo : public SimpleRobot
{
	RobotDrive BackMotors;
	RobotDrive FrontMotors;
	AnalogChannel sonicSensor;
	DigitalInput rightEncA;
	DigitalInput rightEncB;
	DigitalInput leftEncA;
	DigitalInput leftEncB;
	Encoder leftWheels;
	Encoder rightWheels;
	Joystick gamepad;
public:
	RobotDemo(void):
		BackMotors(1, 3),
		FrontMotors(2, 4),
		sonicSensor(1),
		rightEncA(3),
		rightEncB(4),
		leftEncA(1),
		leftEncB(2),
		leftWheels(leftEncA, leftEncB, false, Encoder::k4X),
		rightWheels(rightEncA, rightEncB, false, Encoder::k4X),
		gamepad(1)
	{
		leftWheels.Start();
		rightWheels.Start();
		BackMotors.SetExpiration(0.1);
		FrontMotors.SetExpiration(0.1);
	}
	void Autonomous(void)
	{
		
	}
	void OperatorControl(void)
	{
		BackMotors.SetSafetyEnabled(true);
		FrontMotors.SetSafetyEnabled(true);
		while (IsOperatorControl())
		{
			float leftpower = -gamepad.GetRawAxis(2);
			if(leftpower > -NULLZONE && leftpower < NULLZONE)
				leftpower = 0;
			float rightpower = -gamepad.GetRawAxis(4);
			if(rightpower > -NULLZONE && rightpower < NULLZONE)
				rightpower = 0;
			BackMotors.TankDrive(leftpower,rightpower,0);
			FrontMotors.TankDrive(leftpower,rightpower,0);
			printf("%d %f %d %f %d %d\n", leftWheels.Get(), leftWheels.GetRate(), rightWheels.Get(), rightWheels.GetRate(), sonicSensor.GetValue(), rightEncA.Get());
			Wait(0.005);
		}
	}
};
START_ROBOT_CLASS(RobotDemo);
