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
	Wait(AUTONOMOUSWAITTIME);
	while(IsAutonomous() && IsEnabled() && autonomousTimer.Get() < 5 && pi.Get())
	{
		//digital input is pulled high by default, low means hot
		Wait(0.01);
	}
	printf("Got signal or reached 5s; current time %g; pi %u\n", autonomousTimer.Get(), (unsigned int) pi.Get());
	
	
	double drivestarttime = autonomousTimer.Get();
	//drive up towards shooting range
	if(IsSensorWorking(sonicSensor)){
		while(IsAutonomous() && IsEnabled())
		{
			if(GetDistance(sonicSensor) <= AUTONOMOUSTRIGGERRANGE)
			{
				if(autonomousTimer.Get() >= drivestarttime + AUTONOMOUSMINTIME)
				{
					printf("In range, shooting at time %g\n", autonomousTimer.Get());
					break;
				}
				else{
					printf("Early trigger\n");
					//something's wrong, too early, don't shoot
					break;
				}
				
			}
			if(autonomousTimer.Get() >= drivestarttime + AUTONOMOUSMAXTIME){
				printf("Maxed out\n");
				break;
			}
			
			
			FrontMotors.TankDrive(AUTONOMOUSSPEED, AUTONOMOUSSPEED*DRIVECORRECTION, 0);
			BackMotors.TankDrive(AUTONOMOUSSPEED, AUTONOMOUSSPEED*DRIVECORRECTION, 0);
			Wait(0.01);
		}
	}
	else
	{
		printf("sensor broken\n");
		//sensor's broken
		//rely on the timer
		goingtoshoot = true; //we always get there "in time"
		while(IsAutonomous() && IsEnabled() && autonomousTimer.Get() <= drivestarttime + AUTONOMOUSBACKUPTIME)
		{
			FrontMotors.TankDrive(AUTONOMOUSSPEED, AUTONOMOUSSPEED*DRIVECORRECTION, false);
			BackMotors.TankDrive(AUTONOMOUSSPEED, AUTONOMOUSSPEED*DRIVECORRECTION, false);
			Wait(0.01);
		}
	}
	
	if(goingtoshoot)
	{
		printf("Shooting\n");
		ShootOverride();
		double time = autonomousTimer.Get();
		while(IsAutonomous() && IsEnabled() && autonomousTimer.Get() < time + SHOTTIME)
		{
			//printf("ShooterUpdaate()\n");
			ShooterUpdate();
			Wait(0.01);
		}
	}
	
	//to be sure (in case of the limitswitch braking or moving during auto), stop the chugga here
	shooter.Set(0);
	//brake
	printf("Braking\n");
	FrontMotors.TankDrive(AUTONOMOUSBRAKEPOWER, AUTONOMOUSBRAKEPOWER*DRIVECORRECTION, false);
	BackMotors.TankDrive(AUTONOMOUSBRAKEPOWER, AUTONOMOUSBRAKEPOWER*DRIVECORRECTION, false);
	if (IsAutonomous() && IsEnabled())
		Wait(AUTONOMOUSBRAKETIME);
	
	printf("Stopping\n");
	FrontMotors.TankDrive(0.0, 0.0, false);
	BackMotors.TankDrive(0.0, 0.0, false);
	
	autonomousTimer.Stop();
}
