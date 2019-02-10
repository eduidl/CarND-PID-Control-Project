#ifndef TWIDDLE_H
#define TWIDDLE_H

#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>

#include "PID.h"

class Twiddle {
 public:
  Twiddle();
  virtual ~Twiddle() = default;

  void Process(PID &pid, const double cte);

 private:
  std::array<double, 3> dp_;
  double error_;
  double best_error_;
  double tol_;
  size_t n_move_;
  size_t step_;
  size_t iter_;
  bool first_;
};

Twiddle::Twiddle()
    : dp_({0.01, 0.0001, 0.1}),
      error_(0),
      best_error_(std::numeric_limits<double>::max()),
      tol_(0.0001),
      n_move_(10),
      step_(1),
      iter_(0),
      first_(true) {}

void Twiddle::Process(PID &pid, const double cte) {
  if (std::accumulate(dp_.begin(), dp_.end(), 0.0) < tol_) return;

  error_ += std::pow(cte, 2);

  if (step_ == 1) {
    if (first_) {
      pid.p_[iter_] += dp_[iter_];
    } else {
      pid.p_[iter_] -= 2 * dp_[iter_];
    }
  }

  if (step_ != 0) {
    step_ = (step_ + 1) % n_move_;
    return;
  }

  if (first_) {
    if (error_ < best_error_) {
      best_error_ = error_;
      dp_[iter_] *= 1.1;
    } else {
      error_ = 0.0;
      first_ = false;
      step_ = 1;
      return;
    }
  } else {
    if (error_ < best_error_) {
      best_error_ = error_;
      dp_[iter_] *= 1.1;
    } else {
      pid.p_[iter_] += dp_[iter_];
      dp_[iter_] *= 0.9;
    }
    first_ = true;
  }

  error_ = 0.0;
  step_ = 1;
  iter_ = (iter_ + 1) % 3;

  std::cout << "Kp = " << pid.p_[0] << ", Ki = " << pid.p_[1]
            << ", Kd = " << pid.p_[2] << std::endl;
  return;
}

#endif  // TWIDDLE_H
