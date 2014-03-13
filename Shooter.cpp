#include "MyRobot.h"

void RobotDemo::ShooterUpdate()
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
				//printf("Firing...\n");
				if(!shooterLimit.Get() || shooterTimer.Get() > 1)
				{
					//limit off or safety timer
					readytoshoot = false;
					running = false;
					//printf("Shot fired\n");
				}
			}
			else
			{
				//printf("Reloading...\n");
				//reloading - same thing
				if(shooterLimit.Get() || shooterTimer.Get() > 3)
				{
					//limit on or timer
					readytoshoot = shooterLimit.Get();
					running = false;
					//printf("Reload finished\n");
				}
			}
		}
		
		shooter.Set(running ? SHOOTERSPEED : 0);
	}
}
void RobotDemo::ShootSafe()
{
	if(!readytoshoot)
		ShootOverride();
	
	
	double range = RANGEINSTUPIDINCHES + speed * .57 - speed * speed * .0033;
	
	double dist = GetDistanceInStupidInches(sonicSensor);
	if(dist > range - RANGETOLERANCEINSTUPIDINCHES && dist < range + RANGETOLERANCEINSTUPIDINCHES)
		ShootOverride();
}

void RobotDemo::ShootOverride()
{
	if(running)
		return;
	
	running = true;
	shooterTimer.Reset();
	shooterTimer.Start();
}
