/**
 * particle_filter.cpp
 *
 * Created on: Dec 12, 2016
 * Author: Tiffany Huang
 */

#include "particle_filter.h"

#include <math.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "helper_functions.h"

using std::string;
using std::vector;

void ParticleFilter::init(double x, double y, double theta, double std[]) {
  /**
   * TODO: Set the number of particles. Initialize all particles to 
   *   first position (based on estimates of x, y, theta and their uncertainties
   *   from GPS) and all weights to 1. 
   * TODO: Add random Gaussian noise to each particle.
   * NOTE: Consult particle_filter.h for more information about this method 
   *   (and others in this file).
   */

  num_particles = 10;  // TODO: Set the number of particles


  // Set standard deviations for x, y, and theta
  std::default_random_engine gen;
  double std_x, std_y, std_theta;  // Standard deviations for x, y, and theta
  std_x = std[0];
  std_y = std[1];
  std_theta = std[2];
  // This line creates a normal (Gaussian) distribution for x,y,theta
  std::normal_distribution<double> dist_x(x, std_x);
  std::normal_distribution<double> dist_y(y, std_y);
  std::normal_distribution<double> dist_theta(theta, std_theta);
  
  for (int i = 0; i< num_particles; i++)
  {
    Particle pt;
    
    pt.id = i;
    pt.x  = dist_x(gen);
    pt.y = dist_y(gen);
    pt.theta = dist_theta(gen);
    pt.weight = 1;

    particles.push_back(pt);
  }
  //
  is_initialized = true;
  //

}

void ParticleFilter::prediction(double delta_t, double std_pos[], 
                                double velocity, double yaw_rate) {
  /**
   * TODO: Add measurements to each particle and add random Gaussian noise.
   * NOTE: When adding noise you may find std::normal_distribution 
   *   and std::default_random_engine useful.
   *  http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
   *  http://www.cplusplus.com/reference/random/default_random_engine/
   */

  // Set standard deviations for x, y, and theta
  std::default_random_engine gen;
  double std_x, std_y, std_theta;  // Standard deviations for x, y, and theta
  std_x = std_pos[0];
  std_y = std_pos[1];
  std_theta = std_pos[2];
   //
  for (int i =0; i< particles.size(); i++)
  {
    double x0, y0, theta_0;

    x0 = particles[i].x;
    y0 = particles[i].y;
    theta_0 = particles[i].theta;
    //
    x0 = x0 + velocity/yaw_rate*(sin(theta_0 + yaw_rate * delta_t) - sin(theta_0));
    y0 = y0 + velocity/yaw_rate*(cos(theta_0) - cos(theta_0 + yaw_rate * delta_t));
    theta_0 = theta_0 + yaw_rate * delta_t;
    //
    // This line creates a normal (Gaussian) distribution for x,y,theta
    std::normal_distribution<double> dist_x(x0, std_x);
    std::normal_distribution<double> dist_y(y0, std_y);
    std::normal_distribution<double> dist_theta(theta_0, std_theta);
    //
    particles[i].x = dist_x(gen);
    particles[i].y = dist_y(gen);
    particles[i].theta = dist_theta(gen);
    //
  }
}

void ParticleFilter::dataAssociation(vector<LandmarkObs> predicted, 
                                     vector<LandmarkObs>& observations) {
  /**
   * TODO: Find the predicted measurement that is closest to each 
   *   observed measurement and assign the observed measurement to this 
   *   particular landmark.
   * NOTE: this method will NOT be called by the grading code. But you will 
   *   probably find it useful to implement this method and use it as a helper 
   *   during the updateWeights phase.
   */  
}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[], 
                                   const vector<LandmarkObs> &observations, 
                                   const Map &map_landmarks) {
  /**
   * TODO: Update the weights of each particle using a mult-variate Gaussian 
   *   distribution. You can read more about this distribution here: 
   *   https://en.wikipedia.org/wiki/Multivariate_normal_distribution
   * NOTE: The observations are given in the VEHICLE'S coordinate system. 
   *   Your particles are located according to the MAP'S coordinate system. 
   *   You will need to transform between the two systems. Keep in mind that
   *   this transformation requires both rotation AND translation (but no scaling).
   *   The following is a good resource for the theory:
   *   https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm
   *   and the following is a good resource for the actual equation to implement
   *   (look at equation 3.33) http://planning.cs.uiuc.edu/node99.html
   */
  //
  double weights_sum = 0.0;
  //
  for (int i=0; i< particles.size();i++)
  {
    // covert the observation from VEHICLE's coordinate system to particles map's coordinate system
    double Xp, Yp, theta_p, Xm, Ym, Xc, Yc;

    Xp = particles[i].x;
    Yp = particles[i].y;
    theta_p = particles[i].theta;    
    //
    std::vector<int> associations;
    std::vector<double> sense_x;
    std::vector<double> sense_y;
    //
    associations.clear();
    sense_x.clear();
    sense_y.clear();
    //    
    for(int j=0; j< observations.size(); j++)
    {
      //
      Xc = observations[j].x;
      Yc = observations[j].y;
      // transfrom from VEHICLE's coordinate system to map coordinate system
      Xm = Xp + (cos(theta_p) * Xc) - (sin(theta_p)*Yc);
      Ym = Yp + (sin(theta_p) * Xc) + (cos(theta_p)*Yc);
      // Associate the observation with landmark id
      double dist_min, dist_tmp;
      int id_min = -1;
      //

      for(int k=0; k< map_landmarks.landmark_list.size(); k++)
      {
        dist_tmp = dist(Xm, Ym, map_landmarks.landmark_list[k].x_f, map_landmarks.landmark_list[k].y_f);
        if(k == 0)
        {
          dist_min = dist_tmp;
          id_min = k;
        }
        if(dist_tmp >sensor_range)
        {
          //std::cout<<"Error: the range is out of the sensor range; distance:" <<dist_tmp <<"; sensor range:" <<sensor_range <<std::endl;          
        }
        else if(dist_min > dist_tmp)
        {
          dist_min = dist_tmp;
          id_min = k;
        }
      }
      //
      if(id_min == -1)
      {
        std::cout<<"\n Error to get the association landmark...." << std::endl;
        continue;
      }

      associations.push_back(map_landmarks.landmark_list[id_min].id_i);
      sense_x.push_back(Xm);
      sense_y.push_back(Ym);      
      //
      //caculate the weight
      double mu_x, mu_y, sig_x, sig_y, x_obs, y_obs;
      //
      x_obs = Xm;
      y_obs = Ym;
      sig_x = std_landmark[0];
      sig_y = std_landmark[1];
      mu_x = map_landmarks.landmark_list[id_min].x_f;
      mu_y = map_landmarks.landmark_list[id_min].y_f;
      //
      // calculate normalization term
      double gauss_norm;
      gauss_norm = 1 / (2 * M_PI * sig_x * sig_y);

      // calculate exponent
      double exponent;
      exponent = (pow(x_obs - mu_x, 2) / (2 * pow(sig_x, 2)))
                  + (pow(y_obs - mu_y, 2) / (2 * pow(sig_y, 2)));
        
      // calculate weight using normalization terms and exponent
      double weight;
      weight = gauss_norm * exp(-exponent);
      //update the weight
      particles[i].weight *= weight;
      //
    }
    //Set the associatin observation with landmark
    //std::cout<<"Association: particles[" << i << "] size:" << associations.size() <<" sense_x:" << sense_x.size() << " sense_y:" <<sense_y.size() <<std::endl;
    SetAssociations(particles[i], associations, sense_x, sense_y);
    //
    weights_sum += particles[i].weight;
  }
  
  // Normalize the weights for all particles.  
  for(int i=0; i< particles.size(); i++)
  {
      particles[i].weight = particles[i].weight / weights_sum;
  }  
  //
}

void ParticleFilter::resample() {
  /**
   * TODO: Resample particles with replacement with probability proportional 
   *   to their weight. 
   * NOTE: You may find std::discrete_distribution helpful here.
   *   http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution
   */
  std::vector<double>  weights_tmp;
  int index;
  //init the discrete_distribution's weight

  for (int i=0; i<particles.size();i++)
  {
    //
    weights_tmp.push_back(particles[i].weight);
    //    
  }
  //resampling...
  std::default_random_engine gen;
  std::discrete_distribution<int> disc_dist(weights_tmp.begin(),weights_tmp.end());
  std::vector<Particle> particles_bak = particles;
  //
  particles.clear();
  for (int i=0; i<particles_bak.size();i++)
  {
    index = disc_dist(gen);
    particles.push_back(particles_bak[index]);    
  }
}

void ParticleFilter::SetAssociations(Particle& particle, 
                                     const vector<int>& associations, 
                                     const vector<double>& sense_x, 
                                     const vector<double>& sense_y) {
  // particle: the particle to which assign each listed association, 
  //   and association's (x,y) world coordinates mapping
  // associations: The landmark id that goes along with each listed association
  // sense_x: the associations x mapping already converted to world coordinates
  // sense_y: the associations y mapping already converted to world coordinates
  particle.associations= associations;
  particle.sense_x = sense_x;
  particle.sense_y = sense_y;
}

string ParticleFilter::getAssociations(Particle best) {
  vector<int> v = best.associations;
  std::stringstream ss;
  copy(v.begin(), v.end(), std::ostream_iterator<int>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length()-1);  // get rid of the trailing space
  return s;
}

string ParticleFilter::getSenseCoord(Particle best, string coord) {
  vector<double> v;

  if (coord == "X") {
    v = best.sense_x;
  } else {
    v = best.sense_y;
  }

  std::stringstream ss;
  copy(v.begin(), v.end(), std::ostream_iterator<float>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length()-1);  // get rid of the trailing space
  return s;
}