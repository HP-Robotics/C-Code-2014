#include "MyRobot.h"

void RobotDemo::Autonomous(void)
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
				printf("In range (I guess) %f\n", GetDistanceInStupidInches(sonicSensor));
				goingtoshoot = true;
				break;
			}
			if(autonomousTimer.Get() >= AUTONOMOUSMAXTIME){
				printf("Maxed out\n");
				break;
			}
			FrontMotors.TankDrive(.5, .47, 0);
			BackMotors.TankDrive(.5, .47, 0);
			Wait(0.01);
		}
	}
	else
	{
		printf("sensor broken\n");
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
	FrontMotors.TankDrive(-.15, -.15, 0);
	BackMotors.TankDrive(-.15, -.15, 0);
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
