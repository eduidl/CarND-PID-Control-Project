#include <cmath>
#include <iostream>
#include <string>

#include "PID.h"
#include "json.hpp"
#include <uWS/uWS.h>

// for convenience
using nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  const auto found_null = s.find("null");
  const auto b1 = s.find_first_of("[");
  const auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  } else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main() {
  uWS::Hub h;

  PID pid;
  /**
   * TODO: Initialize the pid variable.
   */

  h.onMessage([&pid](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length,
                     uWS::OpCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length <= 2 || data[0] != '4' || data[1] != '2')
      return;

    const auto s = hasData(std::string(data).substr(0, length));

    if (s == "") {
      // Manual driving
      std::string msg = "42[\"manual\",{}]";
      ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
    }

    const auto j = json::parse(s);

    if (j[0].get<std::string>() != "telemetry")
      return;

    // j[1] is the data JSON object
    const auto cte = std::stod(j[1]["cte"].get<std::string>());
    const auto speed = std::stod(j[1]["speed"].get<std::string>());
    const auto angle = std::stod(j[1]["steering_angle"].get<std::string>());
    double steer_value;
    /**
     * TODO: Calculate steering value here, remember the steering value is
     *   [-1, 1].
     * NOTE: Feel free to play around with the throttle and speed.
     *   Maybe use another PID controller to control the speed!
     */

    // DEBUG
    std::cout << "CTE: " << cte << " Steering Value: " << steer_value
              << std::endl;

    json msgJson;
    msgJson["steering_angle"] = steer_value;
    msgJson["throttle"] = 0.3;
    const auto msg = "42[\"steer\"," + msgJson.dump() + "]";
    std::cout << msg << std::endl;
    ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
  }); // end h.onMessage

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER>, uWS::HttpRequest) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int, char *, size_t) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  constexpr int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }

  h.run();
}
