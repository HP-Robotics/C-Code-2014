#include "WPILib.h"
#include "SmartDashboard/SmartDashboard.h"
#include "math.h"

#define SHOOTERSPEED -.8f
#define RANGEINSTUPIDINCHES 60
#define AUTONOMOUSBACKUPTIME 2
#define AUTONOMOUSMINTIME .5
#define AUTONOMOUSMAXTIME 3

inline bool IsSensorWorking(AnalogChannel& ultrasonic)
{
	return ultrasonic.GetVoltage() > 0.02;
}

inline float GetDistanceInStupidInches(AnalogChannel& ultrasonic)
{
	return ultrasonic.GetVoltage()*80.14 + 1.6456;
}

inline float GetDistanceInCm(AnalogChannel& ultrasonic)
{
	return GetDistanceInStupidInches(ultrasonic) * 2.54f;
}




class RobotDemo : public SimpleRobot
{
	RobotDrive BackMotors;
	RobotDrive FrontMotors;
	AnalogChannel sonicSensor;
	//AnalogChannel sonicSensor2;
	DigitalInput rightEncA;
	DigitalInput rightEncB;
	DigitalInput leftEncA;
	DigitalInput leftEncB;
	Encoder leftWheels;
	Encoder rightWheels;
	Joystick gamepad;
	Compressor comp;
	Solenoid upLoader;
	Solenoid downLoader;
	Jaguar shooter;
	DigitalInput shooterLimit;
	bool isShooting;
	bool isShootingManually;
	DigitalInput pi;
	Timer autonomousTimer;
	double leftStickSpeed;
	double rightStickSpeed;
	bool running;
	bool readytoshoot;
	Timer shooterTimer;

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
		upLoader(1),
		downLoader(2),
		shooter(5),
		shooterLimit(7),
		isShootingManually(false),
		pi(14),
		running(false),
		readytoshoot(false)
		
	{
		
		upLoader.Set(true);
		downLoader.Set(false);
		leftWheels.Start();
		rightWheels.Start();
		BackMotors.SetExpiration(0.1);
		FrontMotors.SetExpiration(0.1);
	}
	
	void ShooterUpdate()
	{
		if(isShootingManually)
		{
			if(gamepad.GetRawButton(6) || gamepad.GetRawButton(8))
			{
				shooter.Set(SHOOTERSPEED);
			}
			else
				shooter.Set(0);
			
			running = false;
			readytoshoot = shooterLimit.Get();
		}
		else
		{
			if (running) {
				if(readytoshoot)
				{
					//firing
					printf("Firing...\n");
					if(!shooterLimit.Get() || shooterTimer.Get() > 1)
					{
						//limit off or safety timer
						readytoshoot = false;
						running = false;
						printf("Shot fired\n");
					}
				}
				else
				{
					printf("Reloading...\n");
					//reloading - same thing
					if(shooterLimit.Get() || shooterTimer.Get() > 3)
					{
						//limit on or timer
						readytoshoot = true;
						running = false;
						printf("Reload finished\n");
					}
				}
			}
			
			shooter.Set(running ? SHOOTERSPEED : 0);
		}
	}
	void ShootSafe()
	{
		if(!readytoshoot)
			ShootOverride();
		
		
		//if(we decide to shoot)
			ShootOverride();
	}
	
	void ShootOverride()
	{
		if(running)
			return;
		
		running = true;
		shooterTimer.Reset();
		shooterTimer.Start();
		
	}
	
	void Autonomous(void)
	{
		
		//setup
		
		comp.Start();
		readytoshoot = shooterLimit.Get();
		bool goingtoshoot = false;
		FrontMotors.SetSafetyEnabled(false);
		BackMotors.SetSafetyEnabled(false);
		shooter.SetSafetyEnabled(false);
		FrontMotors.TankDrive(0.0, 0.0, 0);
		BackMotors.TankDrive(0.0, 0.0, 0);
		shooter.Set(0,0);
		autonomousTimer.Reset();
		autonomousTimer.Start();
		
		//drive up to shooting range
		if(IsSensorWorking(sonicSensor)){
			while(IsAutonomous() && IsEnabled())
			{
				if(GetDistanceInStupidInches(sonicSensor) <= RANGEINSTUPIDINCHES && autonomousTimer.Get() >= AUTONOMOUSMINTIME)
				{
					goingtoshoot = true;
					break;
				}
				if(autonomousTimer.Get() >= AUTONOMOUSMAXTIME)
					break;
				FrontMotors.TankDrive(1, 1, 0);
				BackMotors.TankDrive(1, 1, 0);
				Wait(0.01);
			}
		}
		else
		{
			//sensor's broken
			//rely on the timer
			goingtoshoot = true; //we always get there "in time"
			while(IsAutonomous() && IsEnabled() && autonomousTimer.Get() <= AUTONOMOUSBACKUPTIME)
			{
				FrontMotors.TankDrive(1, 1, 0);
				BackMotors.TankDrive(1, 1, 0);
				Wait(0.01);
			}
		}
		
		//brake
		FrontMotors.TankDrive(-1, -1, 0);
		BackMotors.TankDrive(-1, -1, 0);
		Wait(0.3);
		
		FrontMotors.TankDrive(0.0, 0.0, 0);
		BackMotors.TankDrive(0.0, 0.0, 0);
		
		//if we didn't get there in time we are most likely not in the right position, so we're not going to shoot
		if(goingtoshoot)
		{
			//wait to give the pi opportunity to get a real measurement
			Wait(1);
			
			printf("Waiting for hot signal\n");
			while(IsAutonomous() && IsEnabled() && autonomousTimer.Get() < 5 && pi.Get())
			{
				//digital input is pulled high by default, low means hot
				Wait(0.01);
			}
			
			printf("Saw hot or 5 sec expired\n");
			ShootOverride();
			
			while(IsAutonomous() && IsEnabled()){
				ShooterUpdate();
				Wait(0.01);
			}
		}
		
		autonomousTimer.Stop();
	}
	
	
	
	void OperatorControl(void)
	{
		comp.Start();
		double averageSpeed = 0;
		BackMotors.SetSafetyEnabled(true);
		FrontMotors.SetSafetyEnabled(true);
		shooter.SetSafetyEnabled(true);
		bool wasManualButtonPressed = false;
		bool wasSlowButtonPressed = false;
		int slowMode;
		while (IsOperatorControl() && IsEnabled())
		{
			leftStickSpeed = -pow(gamepad.GetRawAxis(2), 1);
			rightStickSpeed = -pow(gamepad.GetRawAxis(4), 1);
			averageSpeed = avg(leftStickSpeed,rightStickSpeed);
			
			printf("%f (%f)\n", GetDistanceInStupidInches(sonicSensor), sonicSensor.GetVoltage());
			
			
			//DISTANCE TO SMART DASHBOARD
			SmartDashboard::PutNumber("distance",sonicSensor.GetVoltage()/1024);
			
			
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
			
			
			if (gamepad.GetRawButton(6)) //If bringDown or fire pressed, turn the kicker motor
			{
				ShootSafe();
			}
			else if(gamepad.GetRawButton(8))
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
		
		comp.Stop();
		
	}
	inline double avg (double a, double b)
	{
		return (a+b)/2;
	}
};
START_ROBOT_CLASS(RobotDemo);
