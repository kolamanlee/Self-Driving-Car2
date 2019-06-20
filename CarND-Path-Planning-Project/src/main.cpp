#include <uWS/uWS.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Eigen-3.3/Eigen/Core"
#include "Eigen-3.3/Eigen/QR"
#include "helpers.h"
#include "json.hpp"
<<<<<<< HEAD
#include "spline.h"
=======
>>>>>>> fe8562c05fa3cc4196c1d9189e638bbe70256f55

// for convenience
using nlohmann::json;
using std::string;
using std::vector;

int main() {
  uWS::Hub h;

  // Load up map values for waypoint's x,y,s and d normalized normal vectors
  vector<double> map_waypoints_x;
  vector<double> map_waypoints_y;
  vector<double> map_waypoints_s;
  vector<double> map_waypoints_dx;
  vector<double> map_waypoints_dy;

  // Waypoint map to read from
  string map_file_ = "../data/highway_map.csv";
  // The max s value before wrapping around the track back to 0
  double max_s = 6945.554;

  std::ifstream in_map_(map_file_.c_str(), std::ifstream::in);

  string line;
  while (getline(in_map_, line)) {
    std::istringstream iss(line);
    double x;
    double y;
    float s;
    float d_x;
    float d_y;
    iss >> x;
    iss >> y;
    iss >> s;
    iss >> d_x;
    iss >> d_y;
    map_waypoints_x.push_back(x);
    map_waypoints_y.push_back(y);
    map_waypoints_s.push_back(s);
    map_waypoints_dx.push_back(d_x);
    map_waypoints_dy.push_back(d_y);
  }
<<<<<<< HEAD
  //TODO:
  //define the const variables for some limit: speed, jerk
  const double MAX_SPEED = 49.0;
  const double MAX_ACC = 0.224;
  const double VAL_INTERVAL = 0.02; // 0.02 ms * 50 = 1 second
  const double VAL_MILES_PER_HOUR = 2.24;
  //global variables
  double ref_velocity = 0.0; //mph
  int car_lane = 1; // starting at the middle lane

  //END

  h.onMessage([&MAX_SPEED, &MAX_ACC, &VAL_INTERVAL, &VAL_MILES_PER_HOUR,&ref_velocity,&car_lane,&map_waypoints_x,&map_waypoints_y,&map_waypoints_s,
=======

  h.onMessage([&map_waypoints_x,&map_waypoints_y,&map_waypoints_s,
>>>>>>> fe8562c05fa3cc4196c1d9189e638bbe70256f55
               &map_waypoints_dx,&map_waypoints_dy]
              (uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length,
               uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2') {

      auto s = hasData(data);

      if (s != "") {
        auto j = json::parse(s);
        
        string event = j[0].get<string>();
        
        if (event == "telemetry") {
          // j[1] is the data JSON object
          
          // Main car's localization Data
          double car_x = j[1]["x"];
          double car_y = j[1]["y"];
          double car_s = j[1]["s"];
          double car_d = j[1]["d"];
          double car_yaw = j[1]["yaw"];
          double car_speed = j[1]["speed"];

          // Previous path data given to the Planner
          auto previous_path_x = j[1]["previous_path_x"];
          auto previous_path_y = j[1]["previous_path_y"];
          // Previous path's end s and d values 
          double end_path_s = j[1]["end_path_s"];
          double end_path_d = j[1]["end_path_d"];

          // Sensor Fusion Data, a list of all other cars on the same side 
          //   of the road.
          auto sensor_fusion = j[1]["sensor_fusion"];

          json msgJson;

          vector<double> next_x_vals;
          vector<double> next_y_vals;

          /**
           * TODO: define a path made up of (x,y) points that the car will visit
           *   sequentially every .02 seconds
           */
<<<<<<< HEAD
          //
          //use the frenet coordinate and output the global position coordinate system;
          //smooth the cold start/ decrease the jerk on cold start
          //spline the point to smooth(previous path + new path)
          //using sensor fusion to avoid collision(change lanes.. )

          //
          //using sensor fusion to avoid collision(change lanes.. )
          int prev_size = previous_path_x.size();

          /* for debug
          std::cout <<"New Data: car_s = " << car_s <<" car_d=" << car_d << " car_yaw = " << car_yaw 
          << " car_x=" << car_x << " car_y=" << car_y << " prev_size=" << prev_size <<std::endl;
          */

          if (prev_size > 0) {
            car_s = end_path_s;
          }

          bool other_car_left = false;
          bool other_car_right = false;
          bool other_car_ahead = false;
          //
          for(int i=0; i< sensor_fusion.size(); i++)
          {
            float other_car_d = sensor_fusion[i][6];
            float other_car_s = sensor_fusion[i][5];

            int other_car_lane = -1;            
            if(other_car_d>0 && other_car_d<4)
            {
              other_car_lane = 0;
              //
              //
            }
            else if(other_car_d>4 && other_car_d < 8)
            {
              other_car_lane = 1;
            }
            else if(other_car_d> 8 && other_car_d<12)
            {
              other_car_lane = 2;
            }
            //
            if(other_car_lane < 0) 
                continue;
            
            int car_gap = other_car_lane - car_lane;

            switch(car_gap)
            {
              case 0: //same lane;
                other_car_ahead |= other_car_s > car_s && other_car_s - car_s < 30;
                break;
              case -1: //left lane;
                other_car_left |= (car_s - 30 < other_car_s) && (car_s + 30 > other_car_s);
                break;
              case 1:
                other_car_right |= (car_s - 30 < other_car_s) && (car_s + 30 > other_car_s);
                break;
              default:
                break;
            }
          }
          //Next behavior according to left_lane/right_lane and ahead
          //
          double velocity_inc = 0.0;

          if(other_car_ahead)
          {
            if(other_car_left == false && car_lane !=0)
            {
              //there is no car left and can turn to left
              car_lane --;              
            }
            else if(other_car_right == false && car_lane !=2)
            {
              //turn right;
              car_lane ++;
            }
            else{
              // should reduce the velocity;
              velocity_inc -= MAX_ACC;
            }
          }
          else{
            if(ref_velocity < MAX_SPEED)
            {
              velocity_inc += MAX_ACC;
            }        

          }
          //Gernate the trajectory
          double ref_x = car_x;
          double ref_y = car_y;
          double ref_yaw = deg2rad(car_yaw);          

          //spline pts for spline tools
          vector<double> spline_pts_x;
          vector<double> spline_pts_y;
          //
          if(prev_size < 2)
          {
            
            double prev_car_x = car_x - cos(car_yaw);
            double prev_car_y = car_y - sin(car_yaw);
            //
            spline_pts_x.push_back(prev_car_x);
            spline_pts_y.push_back(prev_car_y);
            //
            spline_pts_x.push_back(car_x);
            spline_pts_y.push_back(car_y);
          }
          else
          {
            // use the last previous points.
            ref_x = previous_path_x[prev_size - 1];
            ref_y = previous_path_y[prev_size - 1];
            //
            double prev_car_x2 = previous_path_x[prev_size - 2];
            double prev_car_y2 = previous_path_y[prev_size - 2];
            
            spline_pts_x.push_back(prev_car_x2);
            spline_pts_y.push_back(prev_car_y2);

            spline_pts_x.push_back(ref_x);
            spline_pts_y.push_back(ref_y);

            ref_yaw = atan2(ref_y-prev_car_y2,ref_x-prev_car_x2);
          }
          //generate 3 anchor points for spline
          vector<double> spline_anchor_wp0 = getXY(car_s + 30, 2 + 4*car_lane, map_waypoints_s, map_waypoints_x, map_waypoints_y);
          vector<double> spline_anchor_wp1 = getXY(car_s + 60, 2 + 4*car_lane, map_waypoints_s, map_waypoints_x, map_waypoints_y);
          vector<double> spline_anchor_wp2 = getXY(car_s + 90, 2 + 4*car_lane, map_waypoints_s, map_waypoints_x, map_waypoints_y);

          spline_pts_x.push_back(spline_anchor_wp0[0]);
          spline_pts_y.push_back(spline_anchor_wp0[1]);
          //
          spline_pts_x.push_back(spline_anchor_wp1[0]);
          spline_pts_y.push_back(spline_anchor_wp1[1]);
          //
          spline_pts_x.push_back(spline_anchor_wp2[0]);
          spline_pts_y.push_back(spline_anchor_wp2[1]);
          //
          //convert to the local car's frenet coordinate system
          //std::cout<<"ref_x = " << ref_x << " ref_y = " << ref_y << " ref_yaw= " << ref_yaw << std::endl;

          for(int i=0; i<spline_pts_x.size(); i++)
          {
            //vector<double> frenet_sd = getFrenet(spline_pts_x[i], spline_pts_y[i], ref_yaw, map_waypoints_x, map_waypoints_y);
            //spline_pts_x[i] = frenet_sd[0];
            //spline_pts_y[i] = frenet_sd[1];

            double dx = spline_pts_x[i] - ref_x;
            double dy = spline_pts_y[i] - ref_y;
            //
            spline_pts_x[i] = dx * cos(0 - ref_yaw) - dy * sin(0 - ref_yaw);
            spline_pts_y[i] = dx * sin(0 - ref_yaw) + dy * cos(0 - ref_yaw);
            //
            //std::cout<<"spline_pts[" << i << "] = " << spline_pts_x[i] << "  " << spline_pts_y[i] << std::endl;
            //
          }
          //generate the spline bar;

          tk::spline tk_s;

          tk_s.set_points(spline_pts_x, spline_pts_y);

          //Gnerate all trajectory path's points
          // first use the previous points:
          
          for (int i = 0; i < prev_size; ++i) {            
            next_x_vals.push_back(previous_path_x[i]);
            next_y_vals.push_back(previous_path_y[i]);

          }
          //second, use the new
          double target_s = 30.0; // calculate the 30m ahead
          double target_d = tk_s(target_s);
          double target_dist = sqrt(target_s*target_s+target_d*target_d);
          double s_add_on = 0.0;

          for (int i = prev_size; i < 50; ++i) 
          {
            //
            ref_velocity += velocity_inc;

            if(ref_velocity > MAX_SPEED)
            {
              ref_velocity = MAX_SPEED;
            }
            //
            double N = target_dist/(VAL_INTERVAL*ref_velocity/VAL_MILES_PER_HOUR);
            double s_point = s_add_on + target_s/N;
            double d_point = tk_s(s_point);
            //
            s_add_on = s_point;
            //
            
            //back to gobal coordinate
            double x_point = s_point * cos(ref_yaw) - d_point * sin(ref_yaw);
            double y_point = s_point * sin(ref_yaw) + d_point * cos(ref_yaw);

            x_point += ref_x;
            y_point += ref_y;
            //
            next_x_vals.push_back(x_point);
            next_y_vals.push_back(y_point);
            //
            //std::cout <<"new nex_xy[" << i <<"]: " << x_point <<" " <<y_point <<std::endl;
          }

          //END
=======


>>>>>>> fe8562c05fa3cc4196c1d9189e638bbe70256f55
          msgJson["next_x"] = next_x_vals;
          msgJson["next_y"] = next_y_vals;

          auto msg = "42[\"control\","+ msgJson.dump()+"]";

          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }  // end "telemetry" if
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }  // end websocket if
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