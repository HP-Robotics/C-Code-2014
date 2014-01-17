#include "WPILib.h"

class RobotDemo : public SimpleRobot
{
	RobotDrive robotDrive;
	Joystick gamepad;

public:
	RobotDemo():
		robotDrive(1, 2, 3, 4),
		gamepad(1)
	{
		robotDrive.SetExpiration(0.1);
	}
	void Autonomous()
	{
		robotDrive.SetSafetyEnabled(false);
	}
	void OperatorControl()
	{
		robotDrive.SetSafetyEnabled(true);
		while (IsOperatorControl())
		{
			robotDrive.TankDrive(gamepad.GetY(), gamepad.GetTwist());
			Wait(0.005);
		}
	}
};

START_ROBOT_CLASS(RobotDemo);

