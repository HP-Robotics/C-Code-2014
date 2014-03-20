#ifndef _MYROBOT_H
#define _MYROBOT_H

#include "WPILib.h"
#include "SmartDashboard/SmartDashboard.h"
#include "math.h"

using namespace std;


#define SHOOTERSPEED -.8f
#define RANGEINSTUPIDINCHES 76
#define RANGETOLERANCEINSTUPIDINCHES 4
#define AUTONOMOUSBACKUPTIME 3
#define AUTONOMOUSMINTIME 1
#define AUTONOMOUSMAXTIME 5
#define DISTANCEBUFFERSIZE 10

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
public:
	
	RobotDemo(void);
	void ShooterUpdate();
	void ShootSafe();
	void ShootOverride();
	void Autonomous();
	void OperatorControl();
	
private:
	double GetBufferedDistance();
};

#endif
