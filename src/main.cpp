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

  PID pid;

	std::vector<double> p = {0.5,0.006,3.0};
	std::vector<double> dp = {0.1,0.001,0.5};
	pid.Init(p[0],p[1],p[2]);

	std::cout << std::endl;
	std::cout << "First try: " << p[0] << " " <<  p[1] << " " << p[2] << std::endl; 
	std::cout << "       dp: " << dp[0] << " " << dp[1] << " " << dp[2] << std::endl; 
	std::cout << std::endl;

	double sum_error = 0.0;
	double best_error = 0.0;
	int counter = 1700;
	int param_idx = 0;

	TwiddleState state = TwiddleState::INC;
  /**
   * TODO: Initialize the pid variable.
   */

  h.onMessage([&pid,&sum_error,&best_error,&counter,&param_idx,&p,&dp, &state](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, 
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
          double steer_value;

          /**
           * TODO: Calculate steering value here, remember the steering value is
           *   [-1, 1].
           * NOTE: Feel free to play around with the throttle and speed.
           *   Maybe use another PID controller to control the speed!
           */

					pid.UpdateError(cte);
					double error = pid.TotalError();
					sum_error += pow(error,2.0);
					
					steer_value = error;

					if (steer_value<-0.7) steer_value = -0.7;
					if (steer_value>0.7) steer_value = 0.7;
					
         	double throttle = pid.GetThrottle(speed); 
					counter--;
					if (!counter)
					{
						if (!best_error)
						{
							best_error = sum_error;
							std::cout << "best error was set first time to " << best_error << std::endl;	
						}
						counter = 1700;

						//std::cout << "------------------------------------" << std::endl;
						//std::cout << "A lap passed" << std::endl;
						std::cout << "------------------------------------" << std::endl;
						std::cout << "Actual error: " << sum_error << std::endl;
						std::cout << "param index: " << param_idx << std::endl;
						if (state == TwiddleState::INC_CHECK)
						{
							if (sum_error < best_error)
							{
								best_error = sum_error;
								dp[param_idx] *= 1.1;
								//std::cout << "Incrementing succeeded, dp*1.1=" << dp[param_idx] << std::endl;
								state=TwiddleState::INC;
								// increment param
								param_idx++;
								if (param_idx>2) param_idx=0;
							} else
							{
								//std::cout << "Increment failed, p = " << p[param_idx] << std::endl;
								p[param_idx] -= 2 * dp[param_idx];
								//std::cout << "p = p - 2 * dp  = " << p[param_idx] << std::endl;
								state=TwiddleState::DEC_CHECK;	
							}
						} else if (state==TwiddleState::DEC_CHECK)
						{
							state=TwiddleState::INC;
							//std::cout << "decrementing fork next stage" << std::endl;
							// increment param
							if (sum_error < best_error)
							{
								best_error = sum_error;
								dp[param_idx] *= 1.1;
								//std::cout << "second incrementing succeeded, dp*1.1=" << dp[param_idx] << std::endl;
							} else
							{
								p[param_idx] += dp[param_idx];
								dp[param_idx] *= 0.9;
								//std::cout << "second incrementing failed, dp*0.9=" << dp[param_idx] << std::endl;
							}
							param_idx++;
							if (param_idx>2) param_idx=0;
						}
						if (state==TwiddleState::INC)
						{
							//std::cout << "increment p=" << p[param_idx] << " with dp=" << dp[param_idx] << std::endl;
							p[param_idx] += dp[param_idx];							
							state=TwiddleState::INC_CHECK;
						}
	
						sum_error = 0.0;
						std::cout << std::endl;
						std::cout << "Now trying: " << p[0] << " " << p[1] << " " << p[2]  << std::endl; 
						std::cout << "        dp: " << dp[0] << " " << dp[1] << " " << dp[2] << std::endl; 
						std::cout << std::endl;
						pid.Init(p[0],p[1],p[2]);
						//param_idx++;

					}
          // DEBUG
          //std::cout << "CTE: " << cte << " Steering Value: " << steer_value 
          //          << std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = throttle;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          //std::cout << msg << std::endl;
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
