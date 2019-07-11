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
   * Twiddle  function and the variables
  */
 void Twiddle_Run(double cte);
 void Twiddle_Loop(double cte);
 
 int m_nTwiddle_Run_step;    // for count the loop steps 
 int m_i;                 // for count the loop steps 
 double m_fBest_error;   // keep the best error value in a circle
 double m_fTotal_error; //  for the total error in the  twiddle circle
 int m_nTwiddle_Run_Max; // the max value for twiddle_run loop
 //
 int m_nTwiddle_stage; // the stages: 0 -- p; 1 -- d; 2--i;
 vector<double> m_dp;


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