# **PID Control: PID Controller Parameter** 


### The purpose of this project is to get the best PID parameters to ensure that the car in simulator can safely take round the lake track.The simulator will provide you the Cross Track Error (CTE) and the velocity (mph) in order to compute the appropriate steering angle.

---


**PID Controller**

The goals of this project are the following from the project rubric points:
* goal 1: The PID procedure follows what was taught in the lessons.
* goal 2: Describe the effect each of the P, I, D components had in your implementation.
* goal 3: Describe how the final hyperparameters were chosen.
* goal 4: The vehicle must successfully drive a lap around the track.


## Rubric Points
### Here I considered the [rubric points](https://review.udacity.com/#!/rubrics/1972/view) individually and describe how I addressed each point in my implementation.  

---
### How to achieve the goals

#### 1. Understand the PID

* The P: it is "proportional", that has the most directly effect on the car's behavior. It causes the car to steer proportional (or opposite) to the car's distance from the lane center(CTE); for example, if the car is far to the right it steers hard to the left.

* The D: it is "differential", that counteracts the P component's tendency to ring and overshoot the center line. And a proper D parameter will cause the car to approach the center line smoothly without ringing.

* The I: it is "integral", that counteracts a bias in the CTE which prevents the P-D controller from reaching the center line. This bias can take several forms, such as a steering drift, so the I component particularly are used to reduce the CTE around curves.


#### 2. Implementation for the code

In the lesson, there is a implementation in python examples. so try to write in C++ as the following idea:
* 1). try to have a Car_run() function that similiar to the robot_run to update the p_error, d_error and the i_error and the code in the Car_run() function of PID.cpp.
* 2). try to have a twiddle() function to well-tuned parameters of the PID only when tune the PID after have a manual PID in the Twiddle_Loop() function of PID.cpp.

#### 3. How to tune the parameters

It's diffucult to have a good PID parameter. Once we have a draft PID coeffecients, I use the twiddle to get a better parameters combination by the following method:
* 1). try to print more information by the std::cout to understand in the main.cpp and the twiddle_loop() function of PID.cpp;
* 2). there is a import output information in line#130 of PID.cpp to locate the best error and the PID parameters.
* 3). To try different speed(velocity) with throttle_value, and get the max value of 0.6 for throttle that return to simulator
* 4). To extend the PID for throttle, the I value is zero that can meet the throttle to ensure the car drive safely.

#### 4. Output the PID paramaters

The well-tuned PID parameters are in the lien#44 and #45 of the main.cpp. And also can run it in the simulator to view the car's safety drive.






