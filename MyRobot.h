#ifndef _MYROBOT_H
#define _MYROBOT_H

#ifdef _MSC_VER
#pragma warning(disable:4244)
#pragma warning(disable:4305)
#endif

#include "WPILib.h"
#include "SmartDashboard/SmartDashboard.h"
#include "math.h"
#include "Speed.h"

using namespace std;

// The ratio of the power on the right side of the robot to the power on the left side, to keep it straight. 2 means the right side gets 2x more power
#define DRIVECORRECTION 1.04

#define SHOOTERSPEED -.8f
#define SHOTTIME .6
#define RANGE 68
#define RANGEHALFSPEED 108
#define RANGEFULLSPEED 195
#define RANGEBACKHALFSPEED 36
#define RANGETOLERANCE 8
#define AUTONOMOUSWAITTIME 5.0
#define AUTONOMOUSBACKUPTIME 2
#define AUTONOMOUSMINTIME .8
#define AUTONOMOUSMAXTIME 5
#define AUTONOMOUSSPEED .7
#define AUTONOMOUSBRAKEPOWER -.2
#define AUTONOMOUSBRAKETIME .4
#define DISTANCEBUFFERSIZE 10

inline void PrintSpeed(SpeedCategory speedCategory)
{
	switch(speedCategory)
	{
	case SPEED_ZERO:
		printf("SPEED_ZERO\n");
		break;
	case SPEED_HALF:
		printf("SPEED_HALF\n");
		break;
	case SPEED_FULL:
		printf("SPEED_FULL\n");
		break;
	case SPEED_BACKHALF:
		printf("SPEED_BACKHALF\n");
		break;
	case SPEED_BACKFULL:
		printf("SPEED_BACKFULL\n");
		break;
	default:
		printf("SPEED_CUSTOM\n");
		break;
	}
}

inline bool IsSensorWorking(AnalogChannel& ultrasonic)
{
	return ultrasonic.GetVoltage() > 0.02;
}

inline float GetDistance(AnalogChannel& ultrasonic)
{
	return ultrasonic.GetVoltage()*80.14 + 1.6456;
}

inline float GetDistanceInCm(AnalogChannel& ultrasonic)
{
	return GetDistance(ultrasonic) * 2.54f;
}

inline double avg (double a, double b)
{
	return (a+b)/2;
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
	Timer totalTimer;
	double lastTime;
	double lastDistance;
	double speed;
	double distanceBuffer[DISTANCEBUFFERSIZE];
	int distanceBufferPosition;
	SpeedCategory speedCategory;
public:
	
	RobotDemo(void);
	void ShooterUpdate();
	bool ShootSafe(); //true if shot initiated
	void ShootOverride();

	void Autonomous();
	void OperatorControl();
	void Test();
	virtual void Disabled();
	
private:
	double GetBufferedDistance();
};

#endif
