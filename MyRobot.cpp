#include "WPILib.h"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 

#define NULLZONE .08f

class RobotDemo : public SimpleRobot
{
	RobotDrive BackMotors; // robot drive system
	RobotDrive FrontMotors;
	AnalogChannel Sonic;
	DigitalInput encA;
	DigitalInput encB;
	Encoder LeftWheels;
	Joystick stick; // only joystick

public:
	RobotDemo(void):
		BackMotors(1, 3),	// these must be initialized in the same order
		FrontMotors(2, 4),	// these must be initialized in the same order
		Sonic(1),
		encA(1),
		encB(2),
		LeftWheels(encA, encB, false, Encoder::k4X),
		stick(1)		// as they are declared above.
	{
		LeftWheels.Start();
		BackMotors.SetExpiration(0.1);
		FrontMotors.SetExpiration(0.1);
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
//		myRobot.SetSafetyEnabled(false);
//		FrontWheels.SetSafetyEnabled(false);
//		myRobot.Drive(-0.5, 0.0); 	// drive forwards half speed
//		Wait(2.0); 				//    for 2 seconds
//		myRobot.Drive(0.0, 0.0); 	// stop robot
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		BackMotors.SetSafetyEnabled(true);
		FrontMotors.SetSafetyEnabled(true);
		while (IsOperatorControl())
		{
			float leftpower = -stick.GetRawAxis(2);
			if(leftpower > -NULLZONE && leftpower < NULLZONE)
				leftpower = 0;
			float rightpower = -stick.GetRawAxis(4);
			if(rightpower > -NULLZONE && rightpower < NULLZONE)
				rightpower = 0;
			BackMotors.TankDrive(leftpower,rightpower,0); // drive with arcade style (use right stick)
			FrontMotors.TankDrive(leftpower,rightpower,0); // drive with arcade style (use right stick)
			
			printf("%d %f %d\n", LeftWheels.Get(), LeftWheels.GetRate(), Sonic.GetValue());
			
			Wait(0.005);				// wait for a motor update time
		}
	}
	
	/**
	 * Runs during test mode
	 */
	void Test() {

	}
};

START_ROBOT_CLASS(RobotDemo);

