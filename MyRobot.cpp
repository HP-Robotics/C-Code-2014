#include "WPILib.h"

class RobotDemo : public SimpleRobot
{
	RobotDrive myRobot;
	Joystick stick;

public:
	RobotDemo():
		myRobot(1, 2),
		stick(1)
	{
		myRobot.SetExpiration(0.1);
	}
	
	void Autonomous()
	{
		myRobot.SetSafetyEnabled(false);
		myRobot.Drive(-0.5, 0.0);
		Wait(2.0);
		myRobot.Drive(0.0, 0.0);
	}

	void OperatorControl()
	{
		myRobot.SetSafetyEnabled(true);
		while (IsOperatorControl())
		{
			myRobot.ArcadeDrive(stick);
			Wait(0.005);
		}
	}

	void Test() {

	}
};

START_ROBOT_CLASS(RobotDemo);

