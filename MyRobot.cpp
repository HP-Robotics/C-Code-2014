#include "WPILib.h"
#include "math.h"

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
	Compressor comp;
	Solenoid leftLoader;
	Solenoid rightLoader;
	Jaguar shooter;
	double leftStickSpeed;
	double rightStickSpeed;
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
		gamepad(1),
		comp(1,1),
		leftLoader(1,2),
		rightLoader(3,4),
		shooter(5),
		leftStickSpeed(0),
		rightStickSpeed(0)
	{
		comp.Start();
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
		BackMotors.SetSafetyEnabled(false);
		FrontMotors.SetSafetyEnabled(false);
		leftStickSpeed=-pow(gamepad.GetRawAxis(2), 3);
		rightStickSpeed=-pow(gamepad.GetRawAxis(4), 3);
		while (IsOperatorControl() && IsEnabled())
		{
			if (gamepad.GetRawButton(6)==1)
			{
				leftLoader.Set(true);
				rightLoader.Set(true);
			}
			if (gamepad.GetRawButton(5)==1)
			{
				leftLoader.Set(false);
				rightLoader.Set(false);
			}
			if (gamepad.GetRawButton(7)==1||gamepad.GetRawButton(8)==1)
			{
				shooter.Set(1);
			}
			if (gamepad.GetRawAxis(6)==1)
			{
				BackMotors.TankDrive(1,1,0);
				FrontMotors.TankDrive(1,1,0);
			}
			if (gamepad.GetRawAxis(6)==-1)
			{
				BackMotors.TankDrive(-1,-1,0);
				FrontMotors.TankDrive(-1,-1,0);
			}
			BackMotors.TankDrive(leftStickSpeed,rightStickSpeed,0);
			FrontMotors.TankDrive(leftStickSpeed,rightStickSpeed,0);
			Wait(0.005);
		}
	}
};
START_ROBOT_CLASS(RobotDemo);
