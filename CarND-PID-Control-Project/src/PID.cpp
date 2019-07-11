#include <limits>
#include <iostream>
#include "PID.h"


/**
 * TODO: Complete the PID class. You may add any additional desired functions.
 */

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp_, double Ki_, double Kd_) {
  /**
   * TODO: Initialize PID coefficients (and errors, if needed)
   */
  PID::Kp = Kp_;
  PID::Kd = Kd_;
  PID::Ki = Ki_;
  //
  p_error = d_error = i_error = 0;
  // Twiddling parameters
  m_dp = {0.3*Kp, 0.01*Kd, 0.1*Ki};
  
 
  //
  m_nTwiddle_Run_step = 0;
  m_i = 0;
  m_fBest_error = std::numeric_limits<double>::max();
  m_fTotal_error = 0;
  m_nTwiddle_Run_Max = 10;  
  m_nTwiddle_stage = -1;

}

void PID::Twiddle_Run(double cte)
{
  //
  if(m_nTwiddle_Run_step == 0){
    m_fTotal_error = 0;
  }
  // update PID errors based on cte.
  d_error = cte - p_error;
  p_error = cte;
  i_error += cte;
  //update the error
  if(m_nTwiddle_Run_step>m_nTwiddle_Run_Max/2){
    int n = m_nTwiddle_Run_step + 1 - m_nTwiddle_Run_Max/2;

    m_fTotal_error += cte*cte;
    m_fTotal_error = m_fTotal_error / n;
  }
  //
  m_nTwiddle_Run_step = (m_nTwiddle_Run_step+1)%m_nTwiddle_Run_Max;
  //
  //std::cout<<"Twiddle Run: p_error = " << p_error << " d_error = " << d_error << " i_error = " << i_error <<std::endl;
}

void PID::Twiddle_Loop(double cte)
{
  //
  vector <double> m_p = {PID::Kp, PID::Kd, PID::Ki};

  //run
  Twiddle_Run(cte);

  //waiting for twiddle_run
  if(m_nTwiddle_Run_step == 0){
    //
    m_nTwiddle_stage = (m_nTwiddle_stage+1)% 3;
    //
    if(m_nTwiddle_stage == 0 ){
      m_p[m_i] += m_dp[m_i];
    }    
    //waiting for twiddle_run
    //
    if(m_nTwiddle_stage == 1 || m_nTwiddle_stage == 2){
      if(m_nTwiddle_stage == 1 && m_fTotal_error < m_fBest_error){
        m_fBest_error = m_fTotal_error;
        m_dp[m_i] *= 1.1;
        //
        m_i = (m_i+1) % m_p.size();
      }
      else{
        if(m_nTwiddle_stage == 1){
          m_p[m_i] -= 2*m_dp[m_i];
        }
        //waiting for twiddle_run
        if(m_nTwiddle_stage == 2){
          if(m_fTotal_error < m_fBest_error){
            m_fBest_error = m_fTotal_error;
            m_dp[m_i] *= 1.1;
          }
          else{
            m_p[m_i] += m_dp[m_i];
            m_dp[m_i] *= 0.9;
          }
          //
          m_i = (m_i+1) % m_p.size();
        }
        //        
      }
    }
  }
  //restore
  PID::Kp = m_p[0];PID::Kd = m_p[1];PID::Ki = m_p[2];
  //
  //std::cout<<"Twiddle loop: Kp = " << Kp << " Kd = " << Kd << " Ki = " << Ki <<std::endl;
}

void PID::UpdateError(double cte) {
  /**
   * TODO: Update PID errors based on cte.
   */
  Twiddle_Loop(cte);
  
}

double PID::TotalError() {
  /**
   * TODO: Calculate and return the total error
   */
  return m_fTotal_error;  // TODO: Add your total error calc here!
}
