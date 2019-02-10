#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

#include <uWS/uWS.h>
#include "PID.h"
#include "Twiddle.h"
#include "json.hpp"

// #define TWIDDLE

// for convenience
using nlohmann::json;

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
  pid.Init(0.12099, 0.000902023, 1.099);
  Twiddle twiddle;

  h.onMessage([&pid, &twiddle](uWS::WebSocket<uWS::SERVER> ws, char *data,
                               size_t length, uWS::OpCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length <= 2 || data[0] != '4' || data[1] != '2') return;

    const auto s = hasData(std::string(data).substr(0, length));

    if (s == "") {
      // Manual driving
      std::string msg = "42[\"manual\",{}]";
      ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      return;
    }

    const auto j = json::parse(s);

    if (j[0].get<std::string>() != "telemetry") return;

    // j[1] is the data JSON object
    const auto cte = std::stod(j[1]["cte"].get<std::string>());
#ifdef TWIDDLE
    const auto speed = std::stod(j[1]["speed"].get<std::string>());
#endif

    pid.UpdateError(cte);
    const double steer_value = std::max(std::min(pid.TotalError(), 1.0), -1.0);

#ifdef TWIDDLE
    if (speed > 20.0) twiddle.Process(pid, cte);
#endif

    json msgJson;
    msgJson["steering_angle"] = steer_value;
    msgJson["throttle"] = 0.3;
    const auto msg = "42[\"steer\"," + msgJson.dump() + "]";
    ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
  });  // end h.onMessage

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
