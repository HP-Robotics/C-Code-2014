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
bool RobotDemo::ShootSafe()
{
	if(!readytoshoot){
		ShootOverride();
		return true;
	}
	
	
	/*double range = RANGE + speed * .57 - speed * speed * .0033;
	
	double dist = GetDistance(sonicSensor);
	if(dist > (range - RANGETOLERANCE) && dist < (range + RANGETOLERANCE))
		ShootOverride();*/
	
	double optimalrange = 0;
	
	switch(speedCategory)
	{
	case SPEED_BACKHALF:
		optimalrange = RANGEBACKHALFSPEED;
		break;
	case SPEED_HALF:
		optimalrange = RANGEHALFSPEED;
		break;
	case SPEED_FULL:
		optimalrange = RANGEFULLSPEED;
		break;
	case SPEED_ZERO:
		optimalrange = RANGE;
		break;
	default:
		optimalrange = 0;
		break;
	}
	
	double currentrange = GetBufferedDistance();
	if(currentrange < (optimalrange + RANGETOLERANCE) && currentrange > (optimalrange - RANGETOLERANCE))
	{
		ShootOverride();
		return true;
	}
	else
	{
		printf("NOT - c: %g - o: %g", currentrange, optimalrange);
		PrintSpeed(speedCategory);
		return false;
	}
	
}

void RobotDemo::ShootOverride()
{
	
	printf("SHOT - d: %g ", GetBufferedDistance());
	PrintSpeed(speedCategory);
	if(running)
		return;
	
	running = true;
	shooterTimer.Reset();
	shooterTimer.Start();
}
