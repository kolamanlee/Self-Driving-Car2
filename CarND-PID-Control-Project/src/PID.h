#ifndef PID_H
#define PID_H
#include <vector>
#include <memory>

using namespace std;

class PID {
 public:
  /**
   * Constructor
   */
  PID();

  /**
   * Destructor.
   */
  virtual ~PID();

  /**
   * Initialize PID.
   * @param (Kp_, Ki_, Kd_) The initial PID coefficients
   */
  void Init(double Kp_, double Ki_, double Kd_);

  /**
   * Update the PID error variables given cross track error.
   * @param cte The current cross track error
   */
  void UpdateError(double cte);

  /**
   * Calculate the total PID error.
   * @output The total PID error
   */
  double TotalError();
  
  /**
   * Car_Run(cte) to update the CTE for P, I, D.
   */
  void Car_Run(double cte); // update the CTE to output to the simulator.
  int m_nCar_Run_Max; // the max value for get the error

  /** 
   * Twiddle  function and the variables
  */
 bool m_bOptimize_flag; //if true, run twiddle to get better parameters of pid.


 void Twiddle_Loop();
 
 int m_nCar_Run_step;    // for count the loop steps 
 int m_i;                 // for count the loop steps 
 double m_fBest_error;   // keep the best error value in a circle
 double m_fTotal_error; //  for the total error in the  twiddle circle
 //
 int m_nTwiddle_stage; // the stages: 0 -- p; 1 -- d; 2--i;
 vector<double> m_dp;
  int m_nTwiddle_step; // for get the total steps of running twiddle.

 public:
  /**
   * PID Errors
   */
  double p_error;
  double i_error;
  double d_error;

  /**
   * PID Coefficients
   */ 
  double Kp;
  double Ki;
  double Kd;

};

#endif  // PID_H