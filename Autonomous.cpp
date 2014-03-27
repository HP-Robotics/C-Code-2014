#include "MyRobot.h"

#define AUTONOMOUSTRIGGERRANGE 100

void RobotDemo::Autonomous(void)
{
	//setup
	
	comp.Start();
	readytoshoot = (bool)shooterLimit.Get();
	bool goingtoshoot = false;
	FrontMotors.SetSafetyEnabled(false);
	BackMotors.SetSafetyEnabled(false);
	shooter.SetSafetyEnabled(false);
	FrontMotors.TankDrive(0.0, 0.0, false);
	BackMotors.TankDrive(0.0, 0.0, false);
	shooter.Set(0,0);
	autonomousTimer.Reset();
	autonomousTimer.Start();
	
	//wait 5s or pi signal
	printf("Waiting for hot signal\n");
	while(IsAutonomous() && IsEnabled() && autonomousTimer.Get() < 5 && pi.Get())
	{
		//digital input is pulled high by default, low means hot
		Wait(0.01);
	}
	
	
	
	//drive up towards shooting range
	if(IsSensorWorking(sonicSensor)){
		while(IsAutonomous() && IsEnabled())
		{
			if(GetDistance(sonicSensor) <= AUTONOMOUSTRIGGERRANGE)
			{
				if(autonomousTimer.Get() >= AUTONOMOUSMINTIME)
				{
					printf("In range, shooting\n");
					goingtoshoot = true;
					break;
				}
				else{
					printf("Early trigger\n");
					//something's wrong, too early, don't shoot
					break;
				}
				
			}
			if(autonomousTimer.Get() >= AUTONOMOUSMAXTIME){
				printf("Maxed out\n");
				break;
			}
			
			
			FrontMotors.TankDrive(.8, .8*DRIVECORRECTION, 0);
			BackMotors.TankDrive(.8, .8*DRIVECORRECTION, 0);
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
			FrontMotors.TankDrive(.8*DRIVECORRECTION, .8*DRIVECORRECTION, false);
			BackMotors.TankDrive(.8*DRIVECORRECTION, .8*DRIVECORRECTION, false);
			Wait(0.01);
		}
	}
	
	if(goingtoshoot)
	{
		ShootOverride();
		double time = autonomousTimer.Get();
		while(autonomousTimer.Get() < time + .4)
		{
			printf("ShooterUpdaate()\n");
			ShooterUpdate();
			Wait(0.01);
		}
	}
	
	//to be sure (in case of the limitswitch braking or moving during auto), stop the chugga here
	shooter.Set(0);
	//brake
	FrontMotors.TankDrive(-.15, -.15*DRIVECORRECTION, false);
	BackMotors.TankDrive(-.15, -.15*DRIVECORRECTION, false);
	Wait(0.3);
	
	FrontMotors.TankDrive(0.0, 0.0, false);
	BackMotors.TankDrive(0.0, 0.0, false);
	
	autonomousTimer.Stop();
}
