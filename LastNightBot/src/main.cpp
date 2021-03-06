/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       C:\Users\bcronin                                          */
/*    Created:      Fri Jan 17 2020                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"

using namespace vex;

//possibly add a new DT var to account for different usages
//#define DDT 20 //for the drive (Drive DT)
#define DT 10

// A global instance of vex::brain used for printing to the V5 brain screen
vex::brain       Brain;
// A global instance of vex::competition
vex::competition Competition;

// define your global instances of motors and other devices here

vex::motor    LMotor(vex::PORT2, false);
vex::motor    RMotor(vex::PORT1, true);


//arm definitions
vex::motor* lower_arm;
int lower_arm_len = 2;

vex::motor* upper_arm;
int upper_arm_len = 2;


vex::motor* claw_motors;
int claw_motors_len = 2;


//velocity setpoints
double lower_motor_velocity;
double lower_motor_velocity_goal;

double upper_motor_velocity;
double upper_motor_velocity_goal;

double l_drive_velocity;
double l_drive_velocity_goal;

double r_drive_velocity;
double r_drive_velocity_goal;



vex::controller ctrl(vex::controllerType::primary);


//Function for working with acceleration
double Approach(double current, double goal, double dt) {
    double difference = goal - current;

    if (difference > dt) {
        return current + dt;
    } if (difference < -dt) {
        return current-dt;
    }
    return goal;
}


// BASIC MOVEMENT FUNCTIONS
void moveClaw(double pct)
{
  for(int i = 0; i < claw_motors_len; i++)
  {
    claw_motors[i].spin(directionType::fwd, pct, velocityUnits::pct);
  }
}
void moveLower(double pct)
{
  for(int i = 0; i < lower_arm_len; i++)
  {
    lower_arm[i].spin(directionType::fwd, pct, velocityUnits::pct);
  }
}
void moveUpper(double pct)
{
  for(int i = 0; i < upper_arm_len; i++)
  {
    upper_arm[i].spin(directionType::fwd, pct, velocityUnits::pct);
  }
}
void spinLeft(double pct)
{
  LMotor.spin(directionType::fwd, pct, velocityUnits::pct);
}
void spinRight(double pct)
{
  RMotor.spin(directionType::fwd, pct, velocityUnits::pct);
}


//ADVANCED MOVEMENT FUNCTIONS
void stopDrive()
{
  spinLeft(0);
  spinRight(0);
}
void goRobit(int time)
{
  spinLeft(100);
  spinRight(100);
  task::sleep(time);
  stopDrive();
}
void revRobit(int time)
{
  spinLeft(-100);

  spinRight(-100);
  task::sleep(time);
  stopDrive();
}
void goRobit(double lpwr, double rpwr, int time)
{
  spinLeft(lpwr);
  spinRight(rpwr);
  task::sleep(time);
  stopDrive();
}
void goLower(double pwr, int time)
{
  moveLower(pwr);
  task::sleep(time);
  moveLower(0);
}
void goUpper(double pwr, int time)
{
  moveUpper(pwr);
  task::sleep(time);
  moveUpper(0);
}
void goClaw(double pwr, int time)
{
  moveClaw(pwr);
  task::sleep(time);
  moveClaw(0);
}


//LIST INITIALLIZATIONS
vex::motor* AllocMotorList(vex::motor* list, int size) {
  vex::motor* tmotor_list = (vex::motor*)malloc(sizeof(vex::motor) * size);
  return tmotor_list;
}

void initArmMotorList() {
  lower_arm = AllocMotorList(lower_arm, lower_arm_len);
  lower_arm[0] = vex::motor(PORT13, true); //left side forward
  lower_arm[1] = vex::motor(PORT14, false); //right side reversed

  upper_arm = AllocMotorList(upper_arm, upper_arm_len);
  upper_arm[0] = vex::motor(PORT11, true);
  upper_arm[1] = vex::motor(PORT12, false);
}
void initClawMotorList() {
  claw_motors = AllocMotorList(claw_motors, claw_motors_len);
  claw_motors[0] = vex::motor(PORT19, true);
  claw_motors[1] = vex::motor(PORT20, false);
}


//DRIVE FUNCTIONS
void driveArcade(vex::controller::axis f, vex::controller::axis r) {
  double fwd = f.position();
  double rot = r.position();
  LMotor.spin(directionType::fwd, fwd + rot, velocityUnits::pct);
  RMotor.spin(directionType::fwd, fwd - rot, velocityUnits::pct);
}
void driveTank(vex::controller::axis l, vex::controller::axis r) {
  double ld = l.position(); //gets the true position of the stick in this axis
  double rd = r.position();
  LMotor.spin(directionType::fwd, ld, velocityUnits::pct); //spins the motor in the forward direction at speed ld
  RMotor.spin(directionType::fwd, rd, velocityUnits::pct);
}




//COMPETITION FUNCTIONS
void pre_auton( void ) {
  initArmMotorList();
  initClawMotorList();
}

void auton(bool test) {
  if(!test) {
    // production code here
    revRobit(2333);
    goRobit(1000);
    
    // end production code
  } else {
    // write test code 

    /*BLOCK TWO*/
    // push the block back and then move forward
    revRobit(2333);
    goRobit(1000);
    
    /*BLOCK ONE*/
    // rotate 90 degrees
    goRobit(-100, 100, 500);

    // pinch the claw to grab the cube
    goClaw(100, 300);

    // raise the arm thing
    goUpper(100, 200);

    // rotate another 90 degrees
    goRobit(-100, 100, 500);

    // move robot into cube dropping position
    goRobit(1000);

    // release the claw
    goClaw(-100, 300);

    // back up and reset claw
    revRobit(2000);
    goUpper(-100, 200);

    /*BLOCK THREE*/
    // rotate 90 degrees
    goRobit(100, -100, 500);

    // go forward
    goRobit(500);

    // pinch the claw
    goClaw(100, 300);


    // rotate 90 degrees back
    goRobit(100, -100, 500);

    // drive forward
    goRobit(2000);

    // raise arm
    goUpper(100, 1000);

    //move forward a sliight bit more
    goRobit(200);

    // drop the cube
    goClaw(-100, 300);

    // take it back now y'all
    revRobit(1000);

    //reset
    goUpper(-100, 500);

    // hI I'm maTHmEw ansd i cannae teyp
    // end test code
  }
}

void autonomous( void ) {
  initArmMotorList();
  initClawMotorList();
  auton(false);  // true will run test auton and false will run actual auton
}


//DRIVER CONTROL VARIABLES
bool l1Press = false;
bool l2Press = false;

directionType rDdt;
directionType lDdt;

directionType upperDir;
directionType lowerDir;


//CONTROL FUNCTIONS
void UpdateDriveVel(vex::controller::axis l, vex::controller::axis r, double dt) {
  l_drive_velocity_goal = l.position();
  r_drive_velocity_goal = r.position();

  l_drive_velocity = Approach(l_drive_velocity, l_drive_velocity_goal, dt);
  r_drive_velocity = Approach(r_drive_velocity, r_drive_velocity_goal, dt);

  LMotor.setVelocity(l_drive_velocity, velocityUnits::pct);
  LMotor.spin(directionType::fwd);
  RMotor.setVelocity(r_drive_velocity, velocityUnits::pct);
  RMotor.spin(directionType::fwd);
}

void DriveAccArcade(vex::controller::axis f, vex::controller::axis r) {
  double fwd = f.position();
  double rot = r.position();

  l_drive_velocity_goal = fwd + rot;
  r_drive_velocity_goal = fwd - rot;
}

void UpdateArcadeVelocity(vex::motor l_drive_motor_, vex::motor r_drive_motor_, double dt, directionType direction) {
    // ramp up velocity
    l_drive_velocity = Approach(l_drive_velocity, l_drive_velocity_goal, dt);
    r_drive_velocity = Approach(r_drive_velocity, r_drive_velocity_goal, dt);

    // set motor velocity and spin
    l_drive_motor_.setVelocity(l_drive_velocity, velocityUnits::pct);
    l_drive_motor_.spin(direction);

    r_drive_motor_.setVelocity(r_drive_velocity, velocityUnits::pct);
    r_drive_motor_.spin(direction);
}

void UpdateUpperVelocity(vex::motor* arm_motors_, double dt, directionType direction) {
    // ramp up velocity
    upper_motor_velocity = Approach(upper_motor_velocity, upper_motor_velocity_goal, dt);

    // set motor velocity
    for (int i = 0; i < upper_arm_len; i++) {
      arm_motors_[i].setVelocity(upper_motor_velocity, velocityUnits::pct);
      arm_motors_[i].spin(direction);
    }
}
void UpdateLowerVelocity(vex::motor* arm_motors_, double dt, directionType direction) {
    // ramp up velocity
    lower_motor_velocity = Approach(lower_motor_velocity, lower_motor_velocity_goal, dt);

    // set motor velocity
    for (int i = 0; i < lower_arm_len; i++) {
      arm_motors_[i].setVelocity(lower_motor_velocity, velocityUnits::pct);
      arm_motors_[i].spin(direction);
    }
}



void usercontrol( void ) {
  // User control code here, inside the loop
  initArmMotorList();
  initClawMotorList();

  while (1) {
    //driveTank(ctrl.Axis3, ctrl.Axis2);
    //UpdateDriveVel(ctrl.Axis3, ctrl.Axis2, DT);
    //driveArcade(ctrl.Axis3, ctrl.Axis4);
    DriveAccArcade(ctrl.Axis3, ctrl.Axis4);
    UpdateArcadeVelocity(LMotor, RMotor, DT, directionType::fwd);

    if(ctrl.ButtonR1.pressing()) {
      upper_motor_velocity_goal = -100.0;
      upperDir = directionType::fwd;
    } else if(ctrl.ButtonR2.pressing()) {
      upper_motor_velocity_goal = -100.0;
      upperDir = directionType::rev;
    } else {
      upper_motor_velocity_goal = 0.0;
      for (int i = 0; i < upper_arm_len; i++) {
        upper_arm[i].stop(brakeType::hold);
      }
    }

    UpdateUpperVelocity(upper_arm, DT, upperDir);


    if(ctrl.ButtonL1.pressing()) {
      lower_motor_velocity_goal = -100.0;
      lowerDir = directionType::fwd;
    } else if(ctrl.ButtonL2.pressing()) {
      lower_motor_velocity_goal = -100.0;
      lowerDir = directionType::rev;
    } else {
      lower_motor_velocity_goal = 0.0;
      for (int i = 0; i < lower_arm_len; i++) {
        lower_arm[i].stop(brakeType::hold);
      }
    }

    UpdateLowerVelocity(lower_arm, DT, lowerDir);




    // DON'T DELETE! CLAW MOTOR CODE STUFF HERE
    if(ctrl.ButtonB.pressing() && !l1Press) {
      for(int i = 0; i < claw_motors_len; i++) {
        claw_motors[i].setVelocity(100, velocityUnits::pct);
        claw_motors[i].spin(directionType::fwd);
      }
    } else if(!ctrl.ButtonB.pressing() && l1Press) {
      for(int i = 0; i < claw_motors_len; i++) {
        claw_motors[i].stop(brakeType::brake);
      }
    }
    l1Press = ctrl.ButtonB.pressing();

    if(ctrl.ButtonX.pressing() && !l2Press) {
      for(int i = 0; i < claw_motors_len; i++) {
        claw_motors[i].setVelocity(100, velocityUnits::pct);
        claw_motors[i].spin(directionType::rev);
      }
    } else if(!ctrl.ButtonX.pressing() && l2Press) {
      for(int i = 0; i < claw_motors_len; i++) {
        claw_motors[i].stop(brakeType::hold);
      }
    }
    l2Press = ctrl.ButtonX.pressing();


    vex::task::sleep(20); //Sleep the task for a short amount of time to prevent wasted resources. 
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
    //Set up callbacks for autonomous and driver control periods.
    Competition.autonomous( autonomous );
    Competition.drivercontrol( usercontrol );
    
    //Run the pre-autonomous function. 
    pre_auton();
       
    //Prevent main from exiting with an infinite loop.                        
    while(1) {
      vex::task::sleep(100);//Sleep the task for a short amount of time to prevent wasted resources.
    }    
       
}