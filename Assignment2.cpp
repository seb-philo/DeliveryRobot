#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/Camera.hpp>
#include <webots/TouchSensor.hpp>
#include <webots/GPS.hpp>
#include <webots/InertialUnit.hpp>
#include <webots/DistanceSensor.hpp>

#include <iostream>
#include <cmath>

#define TIME_STEP 64
#define OBSTACLE_THRESHOLD 1000.0

// defines each part of the robot

using namespace webots;

Robot robot;
Motor *leftMotor = robot.getMotor("wheel1");
Motor *rightMotor = robot.getMotor("wheel2");
Camera *camera = robot.getCamera("camera1");
TouchSensor *touchSensor = robot.getTouchSensor("package_sensor");
TouchSensor *bumper = robot.getTouchSensor("bumper");
GPS *gps = robot.getGPS("gps");
InertialUnit *imu = robot.getInertialUnit("IMU");
DistanceSensor *frontSensor = robot.getDistanceSensor("ds_front");
DistanceSensor *leftSensor = robot.getDistanceSensor("ds_left");
DistanceSensor *rightSensor = robot.getDistanceSensor("ds_right");

// defines home location

const double HOME_POSITION_X = 0.0;
const double HOME_POSITION_Y = 0.0;
const double HOME_THRESHOLD = 0.1;

void avoidObstacles() 
{
 // connects distamce sensors
 
 double front_value = frontSensor->getValue();
 double left_value = leftSensor->getValue();
 double right_value = rightSensor->getValue();
    
 // if something is close up front, and closer right than left, it will turn left, vice versa
 
 if (front_value < OBSTACLE_THRESHOLD) 
 {
  if (left_value > right_value) 
  {
   leftMotor->setVelocity(-2.0);
   rightMotor->setVelocity(2.0);
  } 
  else 
  {
   leftMotor->setVelocity(2.0);
   rightMotor->setVelocity(-2.0);
  }
 }
 else if (left_value < OBSTACLE_THRESHOLD) 
 {
  leftMotor->setVelocity(2.0);
  rightMotor->setVelocity(-1.0);
 }
 else if (right_value < OBSTACLE_THRESHOLD) 
 {
  leftMotor->setVelocity(-1.0);
  rightMotor->setVelocity(2.0);
 }
}

void figureEight(double rb_t) 
{
 // robot will turn left for 10s, right for 10s, and stop for 10s
 if (rb_t >= 0 && rb_t < 10) 
 {
  leftMotor->setVelocity(6.0);
  rightMotor->setVelocity(8.7);
 } 
 else if (rb_t >= 10 && rb_t < 20) 
 {
  leftMotor->setVelocity(8.7);
  rightMotor->setVelocity(6.0);
 } 
 else if (rb_t >= 20 && rb_t < 30) 
 {
  leftMotor->setVelocity(0.0);
  rightMotor->setVelocity(0.0);
 }      
}

void findStaff(double rb_t) 
{
 // connects camera
 
 int width = camera->getWidth();
 int height = camera->getHeight();
 const unsigned char *image = camera->getImage();
 
 // defines camera parameters

 int red_pixel_count = 0;
 int left_red_pixels = 0;
 int right_red_pixels = 0;
 int total_pixels = width * height;
 
 // gets rgb data from camera

 for (int x = 0; x < width; x++) 
 {
  for (int y = 0; y < height; y++) 
  {
   int r = Camera::imageGetRed(image, width, x, y);
   int g = Camera::imageGetGreen(image, width, x, y);
   int b = Camera::imageGetBlue(image, width, x, y);
   
   // determines number of red pixels, and whether there is more to the left
   // or right of the camera

   if (r > 200 && g < 100 && b < 100) 
   {  
    red_pixel_count++;
                
    if (x < width / 2) 
    {
     left_red_pixels++;
    } 
    else 
    {
     right_red_pixels++;
    }
   }
  }
 }

 double red_ratio = (double)red_pixel_count / total_pixels;
 
 // if the staff member is close (over half of view), it stops

 if(red_ratio > 0.5) 
 {
  leftMotor->setVelocity(0);
  rightMotor->setVelocity(0);
  return; 
 }

 // if red pixels are present, it will drive towards the side with more 

 if (red_pixel_count > 0) 
 {
  if (left_red_pixels > right_red_pixels) 
  {
   leftMotor->setVelocity(1.0); 
   rightMotor->setVelocity(2.0); 
  } 
  else 
  {
   leftMotor->setVelocity(2.0);  
   rightMotor->setVelocity(1.0);
  }
  
  // avoids obstacles during this
  
  avoidObstacles(); 
 } 
 
 // if no red pixels are present it will drive around 
 // left-right untill it finds them
 
 else 
 {
  if (rb_t >= 0 && rb_t < 8) 
  {
   leftMotor->setVelocity(1);
   rightMotor->setVelocity(1.5);
  } 
  else if (rb_t >= 8 && rb_t < 16) 
  {
   leftMotor->setVelocity(1.5);
   rightMotor->setVelocity(1);
  } 
  else 
  { 
   leftMotor->setVelocity(2);
   rightMotor->setVelocity(2);
  }
  
  // avoids obstacles during this
   
  avoidObstacles();  
 }
}

void returnHome() 
{
 // finds location data
 
 const double *gpsValues = gps->getValues();
 double currentX = gpsValues[0];
 double currentY = gpsValues[1];
 
 // calculates distance to 0,0
    
 double deltaX = HOME_POSITION_X - currentX;
 double deltaY = HOME_POSITION_Y - currentY;
 double distance = sqrt(deltaX * deltaX + deltaY * deltaY);
 
 // if its not at 0,0, it will drive to 0,0
 
 if (distance > HOME_THRESHOLD) 
 {  
  double angleToTarget = atan2(deltaY, deltaX);  
  const double *orientation = imu->getRollPitchYaw(); 
  double currentAngle = orientation[2];  
  double angleDifference = angleToTarget - currentAngle;
  
  // ensures it turns the right way
      
  if (angleDifference > M_PI) 
  {
   angleDifference -= 2 * M_PI;
  } 
  else if (angleDifference < -M_PI) 
  {
   angleDifference += 2 * M_PI;
  }
  
  // if going the correct diraction, it goes foward
  
  if (fabs(angleDifference) < 0.1) 
  {
   leftMotor->setVelocity(2.0); 
   rightMotor->setVelocity(2.0); 
  } 
  
  // corrects the directon
  
  else
  {
   if (angleDifference > 0) 
   {
    leftMotor->setVelocity(-2.0); 
    rightMotor->setVelocity(2.0);  
   } 
   else 
   {
    leftMotor->setVelocity(2.0);  
    rightMotor->setVelocity(-2.0); 
   }
  }
  
  // avoids obstacles during this
  
  avoidObstacles(); 
 } 
 else 
 {
  leftMotor->setVelocity(0.0);
  rightMotor->setVelocity(0.0);  
  
  // avoids obstacles during this
  
  avoidObstacles();
 }
}

void collision()
{
 // if a collision occurs, it drives backwards

 leftMotor->setVelocity(-4.0);
 rightMotor->setVelocity(-4.0); 
}

int main() 
{
 // sets up electronics

 leftMotor->setPosition(INFINITY);
 rightMotor->setPosition(INFINITY);
 leftMotor->setVelocity(0.0);
 rightMotor->setVelocity(0.0);

 touchSensor->enable(TIME_STEP);
 bumper->enable(TIME_STEP);
 camera->enable(TIME_STEP);
 gps->enable(TIME_STEP);
 imu->enable(TIME_STEP);
 frontSensor->enable(TIME_STEP);
 leftSensor->enable(TIME_STEP);
 rightSensor->enable(TIME_STEP);
 
 // gets bumper values, if activates, drives backwards
 
 double bumper_value = bumper->getValue();
 
 // if (bumper_value > 0)
 //    {
 //     collision()
 //    }
 //    else

 while (robot.step(TIME_STEP) != -1) 
 {
 
  // sets timer
  
  double rb_t = fmod(robot.getTime(), 30);  
  double touch_value = touchSensor->getValue();
        
  // if package activates, find staff
  if (touch_value > 20) 
  {
   findStaff(rb_t);
  } 
  
  // ootherwise goes home/does figure of eight
  
  else 
  {
   returnHome();

   const double *gpsValues = gps->getValues();
   double distanceToHome = sqrt(pow(gpsValues[0] - HOME_POSITION_X, 2) + 
                                pow(gpsValues[1] - HOME_POSITION_Y, 2));

   if (distanceToHome < HOME_THRESHOLD) 
   {
    while (touchSensor->getValue() <= 20) 
    {
     rb_t = fmod(robot.getTime(), 30);
     
     figureEight(rb_t);
     robot.step(TIME_STEP);  
    }
   }
  }
 }
}
