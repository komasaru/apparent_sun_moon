#ifndef APPARENT_SUN_MOON_APPARENT_POSITION_HPP_
#define APPARENT_SUN_MOON_APPARENT_POSITION_HPP_

namespace apparent_sun_moon {

struct Position {
  double lambda;
  double beta;
  double d_ec;
  double alpha;
  double delta;
  double d_eq;
  double a_radius;
  double parallax;
};

}  // namespace apparent_sun_moon

#endif

