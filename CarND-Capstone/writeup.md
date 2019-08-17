# Self Driving Car Nanodegree - Capstone Project: System Integration


![carla](./imgs/unity.png)


## Project objectives
The objective of this project is to implement ROS-based core of an autonomous vehicle. The vehicle shall be able to complete a closed-circuit test-track, detecting the traffic lights and stopping whenever required. 


### Specifications

The submitted code must work successfully to navigate Carla around the test track. And meet the following specs in detail.

* Smoothly follow waypoints in the simulator.  
* Respect the target top speed set for the waypoints' twist.twist.linear.x in waypoint_loader.py. 
* Stop at traffic lights when needed.
* Stop and restart PID controllers depending on the state of /vehicle/dbw_enabled.
* Publish throttle, steering, and brake commands at 50hz.


## Individual

I don't attend some group. my udacity's email is kolaman@139.com.


## ROS Architecture
The autonomous driving system is composed of perception, planning and control. The modules communicate according to the following ROS structure of nodes and topics : 
![ROS Architecture](./imgs/ros-architecture.png)


## Perception

I used a deep learning model,named inception-2015 network, to retrain the traffic light images that saved from the simulator.  To get the training images in the simulator, I use the image_cb in tl_dector.py to save the different light pictures. And in totally, there are about 571 pictures to use in the retraining. The examples of pictures are shown as below:

| ![Green](./imgs/green.jpg)  | ![Red](./imgs/red.jpg)  |   ![Yellow](./imgs/yellow.jpg)| ![Unknown](./imgs/unknown.jpg)|




## Planning

The objective of the planning module is to specify target speeds based on mission requirements, road map and scene obstacles. In this project the goal is to keep the center lane, respect the speed limit and stop in the red traffic lights. 

The waypoint planner incorporates the information from the traffic light detection node and the base waypoints and the car's pose to form a final waypoint trajectory. The final trajectory is a list of waypoints with target velocities for the car to follow in the control block. 

If any traffic lights are detected the traffic_cb will contain a valid stop line waypoint in front of the vehicle.  If this stopline is within the lookahead distance, a geometric deceleration trajectory is calculated that is ensured not to exceed the maximum accelleration and jerk contraints. If no red traffic lights are detected the planner will simply return the base waypoints to follow within the speed limit.

## Control

Per requirements, the control module must publish throttle, steering angle and brake torque at 50 Hz. To accomplish this, an  yaw controller provides the steering angle that matches the target linear and angular speeds, taking into account the current linear speed of the vehicle.

The linear speed of the vehicle is controlled with a classic digital PID controller. To avoid any kind of aliasing, the speed tracking error is filtered with a single pole low-pass filter and then fed to the controller.

The controller signal is limited to the vehicle acceleration and deceleration limits. If the control command signals acceleration,the value is sent to the throttle as is. To avoid braking overuse and excessive jerk, the control is configured to first stop sending throttle signals and start actively braking the car only if the required force exceeds the brake deadband. Due to the asymptotic nature of PID control, we need to force a full stop with the parking torque of 700 N.m whenever the speed of vehicle falls below a threshold.

Since the reference signal is relatively smooth, an automatic tuning process was not needed. The manual tuning started with adjusting the proportional gains and comparing it against the first seconds of the reference implementation. The other two components were adjusted with a manual process of minimizing the root mean squared error between the reference implementation and the output. The final result of this process in shown in [Drive-by-wire testing](#Drive-by-wire-testing)

## 

## Build Instructions 

Follow the instruction of the project in the [readme](#readme)

