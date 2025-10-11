# Autonomous Delivery Robot Design and Simulation, with Hospital Application Modelling

Project for the module titled Mechatronics and Robotics, achieving 72%.


<img width="350" height="232" alt="image" src="https://github.com/user-attachments/assets/057e5b05-26e1-4d77-875d-2c5741402ec6" />


<img width="712" height="399" alt="image" src="https://github.com/user-attachments/assets/e471f7a7-e665-4514-a936-bb2f0481269f" />


## Summary
This projected involved the physical design - CAD and standard component selection, the circuit design and microcontroller C++ software development, and the adaption into Webots robotic simulation software, to demonstrate functionality with real world modelling and physics.

## Hospital environment modelling
The simulation required a hospital room to be modelled, with objects, packages and humans modelled to interact with. A set of tasks were to be completed, including following carefully planned routes, as well as free roaming. Additionally, to search out both humans to recieve and transport packages.

## Electronics and Software.
This robot used a range of sensors, including: a force sensor, GPS, IMU, a camera, ultrasonic distance senors and impact sensors; these were paired with electronics such as actuators and microcontrollers, and a state based control system to complete the required tasks.

<img width="600" height="545" alt="Screenshot 2025-03-02 150118" src="https://github.com/user-attachments/assets/ac31ee86-6496-428f-be31-ed8de17f833c" />

This C++ control system was developed both in Tinkercad to demonstrate the Arduino electronics, but also Webots to simulate function.

[`autonomous-delivery.cpp`](https://github.com/seb-philo/DeliveryRobot/blob/main/Assignment2.cpp)
[`arduino-code.cpp`](https://github.com/seb-philo/delivery-robot/blob/main/arduino-code.cpp)

The most significant feature of this to me, was the object recognition used with a camera. The aim was to model a member of hospital staff as a red object, and drive towards them to recieve a package for delivery. This code identified the quantity of red pixels in the frame, determined which of the left and right sides was greater, and used this to lock on and determine the direction to drive.

## Number Recognition Neural Network
Additionally, this project required the use of MATLAB, to create a neural network with a small dataset for number recognition, which could be used to identify specific hospital staff. 

[`number-recognition.m`](https://github.com/seb-philo/DeliveryRobot/blob/main/NumberRecognition4.m)

## Frequency Sensing Circuit
This project also involved a task to create a frequency sensing circuit, using a timing circuit and a series of amplifiers and filters, to detect a frequency to each of us. This I found a challenge, but still interesting.


<img width="736" height="307" alt="image" src="https://github.com/user-attachments/assets/d7a5dc22-420d-4cdf-8dde-f53580dd85d3" />

## CAD Design
Due to time constraints, the CAD design section was brief, but what was created still meets all requirements will considering real world manufacture and assembly. 

<img width="1393" height="1186" alt="Screenshot 2025-03-02 141158" src="https://github.com/user-attachments/assets/ed798f88-513f-4f85-9c66-3221c61a07aa" />


<img width="1346" height="947" alt="Screenshot 2025-03-02 200516" src="https://github.com/user-attachments/assets/5a637a0b-3d76-4e30-92d3-ed3ddcf978a0" />



