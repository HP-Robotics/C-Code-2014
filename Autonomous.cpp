#include "MyRobot.h"

#define AUTONOMOUSTRIGGERRANGE 100

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
			if(GetDistanceInStupidInches(sonicSensor) <= AUTONOMOUSTRIGGERRANGE)
			{
				if(autonomousTimer.Get() >= AUTONOMOUSMINTIME)
				{
					printf("In range, shooting\n");
					ShootOverride();
					//wait before braking
					Wait(.4);
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
			
			
			FrontMotors.TankDrive(.8, .8, 0);
			BackMotors.TankDrive(.8, .8, 0);
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
			FrontMotors.TankDrive(.8, .8, 0);
			BackMotors.TankDrive(.8, .8, 0);
			Wait(0.01);
		}
		ShootOverride();
		Wait(.4);
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
		
		
		
		printf("Saw hot or 5 sec expired\n");
		ShootOverride();
		
		while(IsAutonomous() && IsEnabled()){
			ShooterUpdate();
			Wait(0.01);
		}
	}
	
	autonomousTimer.Stop();
}
