#include "WPILib.h"

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
		//Nuthin
	}
	void OperatorControl(void)
	{
		BackMotors.SetSafetyEnabled(true);
		FrontMotors.SetSafetyEnabled(true);
		while (IsOperatorControl())
		{
			BackMotors.TankDrive(-gamepad.GetRawAxis(2),-gamepad.GetRawAxis(5),0);
			FrontMotors.TankDrive(-gamepad.GetRawAxis(2),-gamepad.GetRawAxis(5),0);
			Wait(0.005);
		}
	}
};
START_ROBOT_CLASS(RobotDemo);
