#include "PID.h"

void PID::Init(const double Kp, const double Ki, const double Kd) {
  p_ = {Kp, Ki, Kd};

  p_error_ = 0;
  i_error_ = 0;
  d_error_ = 0;
}

void PID::UpdateError(double cte) {
  d_error_ = cte - p_error_;  // p_error = previous cte
  p_error_ = cte;
  i_error_ += cte;
}

double PID::TotalError() const {
  return -(p_[0] * p_error_ + p_[1] * i_error_ + p_[2] * d_error_);
}
