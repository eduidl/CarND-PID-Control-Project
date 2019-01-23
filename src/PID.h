#ifndef PID_H
#define PID_H

#include <array>

class PID {
public:
  PID() = default;
  virtual ~PID() = default;

  /**
   * Initialize PID.
   * @param (Kp_, Ki_, Kd_) The initial PID coefficients
   */
  void Init(double Kp_, double Ki_, double Kd_);

  /**
   * Update the PID error variables given cross track error.
   * @param cte The current cross track error
   */
  void UpdateError(double cte);

  /**
   * Calculate the total PID error.
   * @output The total PID error
   */
  double TotalError() const;

  /**
   * PID Coefficients
   */
  std::array<double, 3> p_;

private:
  /**
   * PID Errors
   */
  double p_error_;
  double i_error_;
  double d_error_;
};

#endif // PID_H
