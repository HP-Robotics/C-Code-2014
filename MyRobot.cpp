#include "WPILib.h"
#include "math.h"

#define SHOOTERSPEED .5f

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
	DigitalInput shooterLimit;
	bool isShooting;
	bool isShootingManually;
	DigitalInput pi;
	Timer timer;
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
		comp(5,1),
		leftLoader(1),
		rightLoader(2),
		shooter(5),
		shooterLimit(7),
		pi(14)
		
	{
		comp.Start();
		leftWheels.Start();
		rightWheels.Start();
		BackMotors.SetExpiration(0.1);
		FrontMotors.SetExpiration(0.1);
	}
	
	void ShooterUpdate()
	{
		if(isShootingManually)
		{
			if(gamepad.GetRawButton(7) || gamepad.GetRawButton(8))
			{
				shooter.Set(SHOOTERSPEED);
			}
			else
				shooter.Set(0);
		}
		else
		{
			if(isShooting)
			{
				shooter.Set(SHOOTERSPEED);
				isShooting = !shooterLimit.Get();
			}
			if(!isShooting)
			{
				if(shooterLimit.Get())
					shooter.Set(0);
				else
					shooter.Set(SHOOTERSPEED);
			}
		}
	}
	void ShootSafe()
	{
		//if(we decide to shoot)
			ShootOverride();
	}
	
	void ShootOverride()
	{
		isShooting = true;
	}
	
	void AutonomousMove()
	{
		
		FrontMotors.TankDrive(1, 1, 0);
		BackMotors.TankDrive(1, 1, 0);
	}
	
	void Autonomous(void)
	{
		timer.Reset();
		timer.Start();
		while(timer.Get() < 5 && pi.Get())
		{
			//digital input is pulled high by default, low means hot
		}
		
		ShootOverride();
		timer.Reset();
		timer.Start();
		
		while(IsAutonomous() && IsEnabled())
		{
			ShooterUpdate();
			if(timer.Get() > .4)
				AutonomousMove();
		}
		
	}
	void OperatorControl(void)
	{
		double averageSpeed = 0;
		BackMotors.SetSafetyEnabled(false);
		FrontMotors.SetSafetyEnabled(false);
		bool wasManualButtonPressed = false;
		bool wasSlowButtonPressed = false;
		int slowMode;
		while (IsOperatorControl() && IsEnabled())
		{
			leftStickSpeed = -pow(gamepad.GetRawAxis(2), 3);
			rightStickSpeed = -pow(gamepad.GetRawAxis(4), 3);
			averageSpeed = avg(leftStickSpeed,rightStickSpeed);
			
			//SLOW MODE LOGIC
			if (gamepad.GetRawButton(2))
			{
				if(!wasSlowButtonPressed){
					slowMode=!slowMode;
				}
				wasSlowButtonPressed = true;
			}
			else
				wasSlowButtonPressed = false;
			if (slowMode==true)
			{
				leftStickSpeed=leftStickSpeed*0.5;
				rightStickSpeed=rightStickSpeed*0.5;
				averageSpeed=averageSpeed*0.5;
			}
			
			//LIFTING
			
			if (gamepad.GetRawButton(6))
			{
				leftLoader.Set(true);
				rightLoader.Set(true);
			}
			if (gamepad.GetRawButton(5))
			{
				leftLoader.Set(false);
				rightLoader.Set(false);
			}
			
			//SHOOTING - TODO: Separate Safe and Override
			
			if (gamepad.GetRawButton(7) || gamepad.GetRawButton(8)) //If bringDown or fire pressed, turn the kicker motor
			{
				ShootOverride();
			}
			
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
			if (gamepad.GetRawAxis(6) == 1) //If the dpad arrow up is pushed, full power forwards
			{
				BackMotors.TankDrive(1,1,0);
				FrontMotors.TankDrive(1,1,0);
			}
			else if (gamepad.GetRawAxis(6) == -1) //If the dpad arrow down is pushed, full power backwards
			{
				BackMotors.TankDrive(-1,-1,0);
				FrontMotors.TankDrive(-1,-1,0);
			}
			else if (fabs(averageSpeed) <= 0.8) //Regular speed control if the average of both sticks is less than .8
			{
				BackMotors.TankDrive(leftStickSpeed, rightStickSpeed, 0);
				FrontMotors.TankDrive(leftStickSpeed, rightStickSpeed, 0);
			}
			else //Average speed stabilizer if average of both sticks is greater than .8
			{
				BackMotors.TankDrive(averageSpeed, averageSpeed, 0);
				FrontMotors.TankDrive(averageSpeed, averageSpeed, 0);
			}
			Wait(0.005);
		}
	}
	inline double avg (double a, double b)
	{
		return (a+b)/2;
	}
};
START_ROBOT_CLASS(RobotDemo);
