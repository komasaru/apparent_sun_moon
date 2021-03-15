#ifndef APPARENT_SUN_MOON_CONVERT_HPP_
#define APPARENT_SUN_MOON_CONVERT_HPP_

#include "coord.hpp"
#include "matrix.hpp"

#include <cmath>
#include <iostream>

namespace apparent_sun_moon {

class Convert {
  double eps;

public:
  Convert(double eps): eps(eps) {}
  Coord rect_eq2ec(const Coord);  // 変換: 赤道直交座標 -> 黄道直交座標
  Coord rect_ec2eq(const Coord);  // 変換: 黄道直交座標 -> 赤道直交座標
  Coord rect2pol(  const Coord);  // 変換: 直交座標     -> 極座標
  Coord pol_eq2ec( const Coord);  // 変換: 赤道極座標   -> 黄道極座標
  Coord pol_ec2eq( const Coord);  // 変換: 黄道極座標   -> 赤道極座標
  Coord pol2rect(  const Coord);  // 変換: 極座標       -> 直交座標
};

}  // namespace apparent_sun_moon

#endif

