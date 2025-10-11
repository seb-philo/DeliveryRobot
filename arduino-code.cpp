#define motorPinA 3
#define motorPinB 2
#define motorPinC 5
#define motorPinD 4
#define trigPinF 10
#define echoPinF 11
#define trigPinL 12
#define echoPinL 13
#define trigPinR 8
#define echoPinR 9

#define OBSTACLE_THRESHOLD 1000.0

// defines each part of the robot

// defines camera parameters

int red_pixel_count = 0;
int red_direction = 0;
int red_ratio = 0;

int bumper = 0;

// defines home location

const double HOME_POSITION_X = 0.0;
const double HOME_POSITION_Y = 0.0;
const double HOME_THRESHOLD = 0.1;

double left_speed = 0.0;
double right_speed = 0.0;

int rb_t = -1;

void setup() 
{
 Serial.begin(9600);
  
 // connects sensors 
  
 pinMode(trigPinF, OUTPUT);
 pinMode(echoPinF, INPUT); 
 pinMode(trigPinL, OUTPUT);
 pinMode(echoPinL, INPUT); 
 pinMode(trigPinR, OUTPUT);
 pinMode(echoPinR, INPUT);
 
 //connects motors 
  
 pinMode(motorPinA, OUTPUT);
 pinMode(motorPinB, OUTPUT);
 pinMode(motorPinC, OUTPUT);
 pinMode(motorPinD, OUTPUT);
}

void figureEight()
{
 
 // robot will turn left for 10s, right for 10s, and stop for 10s
  
 if (rb_t >= 0 && rb_t < 10) 
 {
  left_speed = 100;
  right_speed = 200;
  Serial.println("State 2");
 } 
 else if (rb_t >= 10 && rb_t < 20) 
 {
  left_speed = 200;
  right_speed = 100;
  Serial.println("State 2");
 } 
 else if (rb_t >= 20 && rb_t < 30)
 {
  left_speed = LOW;
  right_speed = LOW;
   
  Serial.println("State 1");
 }    
 analogWrite(motorPinA, left_speed); 
 digitalWrite(motorPinB, LOW);  
 analogWrite(motorPinC, right_speed);
 digitalWrite(motorPinD, LOW);
}

void findStaff()
{ 
 // connects camera
  
 int red_pixel_count = digitalRead(A1);
 int red_direction = digitalRead(A2);
 int red_ratio = digitalRead(0);
  
 // determines number of red pixels, and whether there is more to the left
 // or right of the camera
  
 if(rb_t > 30)
 {
  rb_t = 0;
 }
  
 // if the staff member is close (over half of view), it stops
  
 if(red_ratio > 0.5) 
 {
  stop();
  Serial.println("State 4");
 }
  
 // if red pixels are present, it will drive towards the side with more
  
 else if (red_pixel_count > 0) 
 {
  Serial.println("State 3");
  if (red_direction > 0) 
  {
   left_speed = 100;
   right_speed = 200;
  } 
  else 
  {
   left_speed = 200;
   right_speed = 100;
  }
   
  // avoids obstacles during this
   
  avoidObstacles(); 
 } 
  
 // if no red pixels are present it will drive around 
 // left-right untill it finds them
  
 else 
 {
  Serial.println("State 3");
  if (rb_t >= 0 && rb_t <= 8) 
  {
   left_speed = 100;
   right_speed = 150;
  } 
  else if (rb_t > 8 && rb_t <= 16) 
  {
   left_speed = 150;
   right_speed = 100;
  } 
  else if (rb_t > 16 && rb_t <= 30)
  { 
   left_speed = 200;
   right_speed = 200;
  }
   
  // avoids obstacles during this
   
  avoidObstacles(); 
 }
 
 analogWrite(motorPinA, left_speed); 
 digitalWrite(motorPinB, LOW);  
 analogWrite(motorPinC, right_speed);
 digitalWrite(motorPinD, LOW);
}

void returnHome()
{ 
 Serial.println("State 6");
  
 // finds location data
  
 int pot1 = analogRead(A3);
 int pot2 = analogRead(A4);
 int pot3 = analogRead(A5);
  
 // calculates distance to 0,0
  
 int currentX = map(pot1,0,1023,-5,5);
 int currentY = map(pot2,0,1023,-5,5);
 int orientation = map(pot3,0,1023,-180,180);
    
 double deltaX = HOME_POSITION_X - currentX;
 double deltaY = HOME_POSITION_Y - currentY;
 double distance = sqrt(deltaX * deltaX + deltaY * deltaY);
  
 // if its not at 0,0, it will drive to 0,0
   
 if (distance > HOME_THRESHOLD) 
 {  
  double angleToTarget = atan2(deltaY, deltaX);  
  double angleDifference = angleToTarget - orientation;
   
  // ensures it turns the right way

  if (angleDifference > M_PI) 
  {
   angleDifference -= 2 * M_PI;
  } 
  else if (angleDifference < -M_PI) 
  {
   angleDifference += 2 * M_PI;
  }
   
   // if going the correct diraction, it goes forward
   
  if (angleDifference < 10 && angleDifference > -10) 
  {
   left_speed = 200;
   right_speed = 200;  
  } 
   
  // corrects the directon
   
  else 
  {
   if (angleDifference > 0) 
   {
    left_speed = -200;
    right_speed = 200;  
   } 
   else 
   {
    left_speed = 200;
    right_speed = -200;
   }
  }
 } 
 else
 {
  stop();
 }
  
 // avoids obstacles during this
   
 avoidObstacles();
  
 analogWrite(motorPinA, left_speed); 
 digitalWrite(motorPinB, LOW);  
 analogWrite(motorPinC, right_speed);
 digitalWrite(motorPinD, LOW); 
}

void collision()
{
 // if a collision occurs, it drives backwards
  
 Serial.println("State 5");
  
 analogWrite(motorPinA, 400); 
 digitalWrite(motorPinB, HIGH);  
 analogWrite(motorPinC, 400);
 digitalWrite(motorPinD, HIGH);
  
 delay(1000);
}

void stop()
{
 // stops the robot
  
 left_speed = LOW;
 right_speed = LOW;
 // State 1 & 4
}

void avoidObstacles()
{  
 long durationF;
 long durationL;
 long durationR;
  
 // connects distance sensors
  
 digitalWrite(trigPinF, LOW);
 delayMicroseconds(2);
 digitalWrite(trigPinF, HIGH);
 delayMicroseconds(10);
 digitalWrite(trigPinF, LOW);
 durationF = pulseIn(echoPinF, HIGH);
 int distanceF = map(durationF,0,19280,0,1000);
  
 digitalWrite(trigPinL, LOW);
 delayMicroseconds(2);
 digitalWrite(trigPinL, HIGH);
 delayMicroseconds(10);
 digitalWrite(trigPinL, LOW);
 durationL = pulseIn(echoPinL, HIGH);
 int distanceL = map(durationL,0,19280,0,1000);

 digitalWrite(trigPinR, LOW);
 delayMicroseconds(2);
 digitalWrite(trigPinR, HIGH);
 delayMicroseconds(10);
 digitalWrite(trigPinR, LOW);
 durationR = pulseIn(echoPinR, HIGH);
 int distanceR = map(durationR,0,19280,0,1000);
  
 // if something is close up front, and closer right than left, it will turn left, vice versa
  
 if (distanceF < OBSTACLE_THRESHOLD) 
 {
  if (distanceL > distanceR) 
  {
   left_speed = 200;
   right_speed = -200;
  } 
  else 
  {
   left_speed = -200;
   right_speed = 200;
  }
 }
 else if (distanceL < OBSTACLE_THRESHOLD) 
 {
  left_speed = -100;
  right_speed = 200;
 }
 else if (distanceR < OBSTACLE_THRESHOLD) 
 {
  left_speed = 200;
  right_speed = -100;
 }
  
 analogWrite(motorPinA, left_speed); 
 digitalWrite(motorPinB, LOW);  
 analogWrite(motorPinC, right_speed);
 digitalWrite(motorPinD, LOW);
}


void loop() 
{
 // timer 
  
 rb_t = rb_t + 1;
 delay(1000);
 
 if(rb_t > 30)
 {
  rb_t = 0;
 }
  
 // gets bumper values, if activates, drives backwards
  
 int bumper = digitalRead(7);
 int touch_value = analogRead(A0);
 
 int touchValue = map(touch_value,0,1023,0,50);
  
 int pot1 = analogRead(A3);
 int pot2 = analogRead(A4);
 int pot3 = analogRead(A5);
  
 int currentX = map(pot1,0,1023,-5,5);
 int currentY = map(pot2,0,1023,-5,5);
 int orientation = map(pot3,0,1023,0,360);
  
 double distanceToHome = sqrt(pow(currentX - HOME_POSITION_X, 2) + 
                               pow(currentY - HOME_POSITION_Y, 2));
  
 // if bumper activates, reverses
  
 if (bumper == 1)
 {
  collision();
  // State 5
 }
 else
   
 // if package activates, find staff
   
 {
  if (touchValue > 20) 
  {
   findStaff();
   // State 3
  } 
   
     // ootherwise goes home/does figure of eight

   
  else if (touchValue < 20 && distanceToHome > HOME_THRESHOLD)
  {
   returnHome();
   // State 6
  }
  else
  {
   figureEight();
   //State 1
  }
 }
}
