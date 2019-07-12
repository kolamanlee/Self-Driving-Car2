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
  p_error = std::numeric_limits<double>::max(); 
  d_error = i_error = 0;

  m_nCar_Run_step = 0;
  m_nCar_Run_Max = 100; 

  // Twiddling parameters
  m_bOptimize_flag = false;
  m_dp = {0.2*Kp, 0.2*Kd, 0.2*Ki};     
  m_i = 0;
  m_fBest_error = std::numeric_limits<double>::max();
  m_fTotal_error = 0;
  //
  m_nTwiddle_step = 0;
  // 
  m_nTwiddle_stage = -1;

}

void PID::Car_Run(double cte)
{
  if(p_error == std::numeric_limits<double>::max()){
    //first time 
    p_error = cte;
  }
  if(m_nCar_Run_step == 0){
    m_fTotal_error = 0;
  }
  // update PID errors based on cte.
  d_error = cte - p_error;
  p_error = cte;
  i_error += cte;
  //update the error
  if(m_nCar_Run_step>m_nCar_Run_Max/2){
    m_fTotal_error += cte*cte;
  }
  //
  m_nCar_Run_step = (m_nCar_Run_step+1)%m_nCar_Run_Max;
  //
  //std::cout<<"Twiddle Run: p_error = " << p_error << " d_error = " << d_error << " i_error = " << i_error <<std::endl;
}

void PID::Twiddle_Loop()
{
  //
  vector <double> m_p = {PID::Kp, PID::Kd, PID::Ki};

  
  m_nTwiddle_stage = (m_nTwiddle_stage+1)% 3;
  
  //
  if(m_nTwiddle_stage == 0 ){
    m_p[m_i] += m_dp[m_i];
    std::cout <<" Twiddle loop stage1 = " << m_nTwiddle_stage << " m_i = " << m_i << std::endl;  
  }    
  //waiting for twiddle_run
  //
  if(m_nTwiddle_stage == 1 || m_nTwiddle_stage == 2){
    if(m_nTwiddle_stage == 1 && m_fTotal_error < m_fBest_error){
      m_fBest_error = m_fTotal_error;
      m_dp[m_i] *= 1.1;
      //
      std::cout <<" Twiddle loop stage2 = " << m_nTwiddle_stage << " m_i = " << m_i <<"m_p="<<m_p[m_i]<<std::endl;

      m_i = (m_i+1) % m_p.size();
      m_nTwiddle_stage = -1; // new P or I or D, new stage
    }
    else{
      if(m_nTwiddle_stage == 1){
        m_p[m_i] -= 2*m_dp[m_i];
        std::cout <<" Twiddle loop stage3 = " << m_nTwiddle_stage << " m_i = " << m_i <<"m_p="<<m_p[m_i]<<std::endl;
      }
      //waiting for twiddle_run
      if(m_nTwiddle_stage == 2){
        if(m_fTotal_error < m_fBest_error){
          m_fBest_error = m_fTotal_error;
          m_dp[m_i] *= 1.1;
          std::cout <<" Twiddle loop stage4 = " << m_nTwiddle_stage << " m_i = " << m_i <<"m_p="<<m_p[m_i]<<std::endl;
        }
        else{
          m_p[m_i] += m_dp[m_i];
          m_dp[m_i] *= 0.9;
          std::cout <<" Twiddle loop stage5 = " << m_nTwiddle_stage << " m_i = " << m_i <<"m_p="<<m_p[m_i]<<std::endl;
        }
        //
        m_i = (m_i+1) % m_p.size();
        m_nTwiddle_stage = -1; // new P or I or D, new stage
      }
      //        
    }
  }
  //restore
  PID::Kp = m_p[0];PID::Kd = m_p[1];PID::Ki = m_p[2];

}

void PID::UpdateError(double cte) {
  /**
   * TODO: Update PID errors based on cte.
   */

  //run
  Car_Run(cte);
  //
  if(m_bOptimize_flag){    
    m_nTwiddle_step++;
    if(m_nCar_Run_step == 0){
        Twiddle_Loop();
        if(m_fTotal_error <= m_fBest_error){
          std::cout<<"Twiddle good paramater: steps = " << m_nTwiddle_step << " Kp = " << Kp << " Kd = " << Kd << " Ki = " << Ki <<std::endl;
        }
    }
  }
  
}

double PID::TotalError() {
  /**
   * TODO: Calculate and return the total error
   */
  return m_fTotal_error;  // TODO: Add your total error calc here!
}
