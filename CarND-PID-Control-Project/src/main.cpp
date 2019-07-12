#include <math.h>
#include <uWS/uWS.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include "PID.h"

// for convenience
using nlohmann::json;
using std::string;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
string hasData(string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != string::npos) {
    return "";
  }
  else if (b1 != string::npos && b2 != string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main() {
  uWS::Hub h;

  PID pid_s,pid_t;
  /**
   * TODO: Initialize the pid variable.
   */
  //pid_s.Init(0.1, 0.0005, 4);
  //pid_s.Init(0.161829,0.000449963,4.40872);
  //pid_s.Init(0.308298,0.000747839,7.32729);
  //pid_s.Init(0.369958,0.000747839,7.32729); //throttle_value = 0.3
  pid_s.Init(0.44395,0.000747839,5.86183); //throttle_value = 0.6
  //
  pid_t.Init(0.44395,0.000,5.86183); //throttle_value = 0.6

  h.onMessage([&pid_s,&pid_t](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, 
                     uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2') {
      auto s = hasData(string(data).substr(0, length));

      if (s != "") {
        auto j = json::parse(s);

        string event = j[0].get<string>();

        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<string>());
          double speed = std::stod(j[1]["speed"].get<string>());
          double angle = std::stod(j[1]["steering_angle"].get<string>());
          double steer_value, throttle_value=0.6;
          /**
           * TODO: Calculate steering value here, remember the steering value is
           *   [-1, 1].
           * NOTE: Feel free to play around with the throttle and speed.
           *   Maybe use another PID controller to control the speed!
           */
          pid_s.UpdateError(cte);
  
          steer_value = - pid_s.Kp*pid_s.p_error - pid_s.Kd*pid_s.d_error - pid_s.Ki*pid_s.i_error;
          if(steer_value > pi()/4){
            steer_value = pi()/4;
          }
          if(steer_value <-pi()/4){
            steer_value = -pi()/4;
          }

          // DEBUG
          std::cout << "CTE_Steer: " << cte << " Steering Value: " << steer_value   
             <<" P: " << pid_s.Kp << ", " << pid_s.p_error
             <<" I: " << pid_s.Ki << ", " <<pid_s.i_error
             <<" D: " << pid_s.Kd << ", " << pid_s.d_error
             <<" total error: " << pid_s.TotalError() 
             <<" best error:" << pid_s.m_fBest_error
             <<std::endl;

          pid_t.UpdateError(fabs(cte));
          throttle_value = 0.60 - pid_t.Kp * pid_t.p_error - pid_t.Kd * pid_t.d_error  - pid_t.Ki * pid_t.i_error;          
          // DEBUG
          std::cout << "CTE_Throttle: " << cte << " Steering Value: " << steer_value   
             <<" P: " << pid_t.Kp << ", " << pid_t.p_error
             <<" I: " << pid_t.Ki << ", " <<pid_t.i_error
             <<" D: " << pid_t.Kd << ", " << pid_t.d_error
             <<" total error: " << pid_t.TotalError() 
             <<" best error:" << pid_t.m_fBest_error
             <<std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = throttle_value;//0.3;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }  // end "telemetry" if
      } else {
        // Manual driving
        string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }  // end websocket message if
  }); // end h.onMessage

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, 
                         char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  
  h.run();
}