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
		int slowMode;
		double averageSpeed=0;
		BackMotors.SetSafetyEnabled(false);
		FrontMotors.SetSafetyEnabled(false);
		while (IsOperatorControl() && IsEnabled())
		{
			leftStickSpeed=-pow(gamepad.GetRawAxis(2), 3);
			rightStickSpeed=-pow(gamepad.GetRawAxis(4), 3);
			//averageSpeed = (leftStickSpeed+rightStickSpeed)/2;
			averageSpeed = avg(leftStickSpeed,rightStickSpeed);
			if (gamepad.GetRawButton(2)==1)
			{
				slowMode=!slowMode;
			}
			if (slowMode==true)
			{
				leftStickSpeed=leftStickSpeed*0.5;
				rightStickSpeed=rightStickSpeed*0.5;
				averageSpeed=averageSpeed*0.5;
			}
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
			if (gamepad.GetRawButton(7)==1||gamepad.GetRawButton(8)==1) //If bringDown or fire pressed, turn the kicker motor
			{
				shooter.Set(1);
			}
			if (gamepad.GetRawAxis(6)==1) //If the dpad arrow up is pushed, full power forwards
			{
				BackMotors.TankDrive(1,1,0);
				FrontMotors.TankDrive(1,1,0);
			}
			else if (gamepad.GetRawAxis(6)==-1) //If the dpad arrow down is pushed, full power backwards
			{
				BackMotors.TankDrive(-1,-1,0);
				FrontMotors.TankDrive(-1,-1,0);
			}
			else if (fabs(averageSpeed)<= 0.8) //Regular speed control if the average of both sticks is less than .8
			{
				BackMotors.TankDrive(leftStickSpeed,rightStickSpeed,0);
				FrontMotors.TankDrive(leftStickSpeed,rightStickSpeed,0);
			}
			else //Average speed stabilizer if average of both sticks is greater than .8
			{
				BackMotors.TankDrive(averageSpeed,averageSpeed,0);
				FrontMotors.TankDrive(averageSpeed,averageSpeed,0);
			}
			Wait(0.005);
		}
	}
	double avg (double a, double b)
	{
		double z = (a+b/2);
		return z;
	}
};
START_ROBOT_CLASS(RobotDemo);
