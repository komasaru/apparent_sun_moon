#include "convert.hpp"

namespace apparent_sun_moon {

// 定数
static constexpr double kPi  = atan(1.0) * 4;
static constexpr double kPi2 = kPi * 2;

/*
 * @brief      変換: 赤道直交座標 -> 黄道直交座標
 *
 * @param[in]  赤道直交座標 (Coord)
 * @return     黄道直交座標 (Coord)
 */
Coord Convert::rect_eq2ec(const Coord pos_src) {
  double mtx[3][3];  // 回転行列
  Coord  pos_res;    // x, y, z

  try {
    if (!r_x(eps, mtx)) {
      std::cout << "[ERROR] Failed to generate a rotation matrix!";
      return pos_res;
    }
    pos_res = rotate(pos_src, mtx);
  } catch (...) {
    throw;
  }

  return pos_res;
}

/*
 * @brief      変換: 黄道直交座標 -> 赤道直交座標
 *
 * @param[in]  黄道道直交座標 (Coord)
 * @return     赤道直交座標 (Coord)
 */
Coord Convert::rect_ec2eq(const Coord pos_src) {
  double mtx[3][3];  // 回転行列
  Coord  pos_res;    // x, y, z

  try {
    if (!r_x(-eps, mtx)) {
      std::cout << "[ERROR] Failed to generate a rotation matrix!";
      return pos_res;
    }
    pos_res = rotate(pos_src, mtx);
  } catch (...) {
    throw;
  }

  return pos_res;
}

/*
 * @brief      変換: 直交座標 -> 極座標
 *
 * @param[in]  直交座標 (Coord)
 * @return     極座標 (Coord)
 */
Coord Convert::rect2pol(const Coord pos_src) {
  double l;        // lambda
  double p;        // phi
  double r;        // radius
  double d;        // radius for work
  Coord  pos_res;  // lambda, phi, radius

  try {
    // lambda
    d = sqrt(pos_src.x * pos_src.x + pos_src.y * pos_src.y);
    l = atan2(pos_src.y, pos_src.x);
    while (l < 0.0) l += kPi2;
    // phi
    p = atan2(pos_src.z, d);
    // radius
    r = sqrt(
        pos_src.x * pos_src.x
      + pos_src.y * pos_src.y
      + pos_src.z * pos_src.z
    );
    pos_res = {l, p, r};
  } catch (...) {
    throw;
  }

  return pos_res;
}

/*
 * @brief      変換: 赤道極座標 -> 黄道極座標
 *
 * @param[in]  赤道極座標 (Coord)
 * @return     黄道極座標 (Coord)
 */
Coord Convert::pol_eq2ec(const Coord pos_src) {
  double l;        // lambda
  double b;        // beta(phi)
  double v1;       // for work
  double v2;       // for work
  Coord  pos_res;  // lambda, beta, radius

  try {
    // lambda
    v1 = sin(pos_src.y) * sin(eps)
       + cos(pos_src.y) * sin(pos_src.x) * cos(eps);
    v2 = cos(pos_src.y) * cos(pos_src.x);
    l = atan2(v1, v2);
    while (l < 0.0) l += kPi2;
    // beta
    v1 = sin(pos_src.y) * cos(eps)
       - cos(pos_src.y) * sin(pos_src.x) * sin(eps);
    b = asin(v1);
    pos_res = {l, b, pos_src.z};  // radius は変更なし
  } catch (...) {
    throw;
  }

  return pos_res;
}

/*
 * @brief      変換: 黄道極座標 -> 赤道極座標
 *
 * @param[in]  黄道極座標 (Coord)
 * @return     赤道極座標 (Coord)
 */
Coord Convert::pol_ec2eq(const Coord pos_src) {
  double a;        // alpha(lambda)
  double d;        // delta(phi)
  double v1;       // for work
  double v2;       // for work
  Coord  pos_res;  // alpha, delta, radius

  try {
    // alpha
    v1 = -sin(pos_src.y) * sin(eps)
       +  cos(pos_src.y) * sin(pos_src.x) * cos(eps);
    v2 =  cos(pos_src.y) * cos(pos_src.x);
    a = atan2(v1, v2);
    while (a < 0.0) a += kPi2;
    // delta
    v1 = sin(pos_src.y) * cos(eps)
       + cos(pos_src.y) * sin(pos_src.x) * sin(eps);
    d = asin(v1);
    pos_res = {a, d, pos_src.z};  // radius は変更なし
  } catch (...) {
    throw;
  }

  return pos_res;
}

/*
 * @brief      変換: 極座標 -> 直交座標
 *
 * @param[in]  極座標 (Coord)
 * @return     直交座標 (Coord)
 */
Coord Convert::pol2rect(const Coord pos_src) {
  double mtx_y[3][3];  // 回転行列
  double mtx[3][3];    // 回転行列
  Coord  pos_res;      // x, y, z

  try {
    if (!r_y(pos_src.y, mtx_y)) {
      std::cout << "[ERROR] Failed to generate a rotation matrix!";
      return pos_res;
    }
    if (!r_z(-pos_src.x, mtx, mtx_y)) {
      std::cout << "[ERROR] Failed to generate a rotation matrix!";
      return pos_res;
    }
    pos_res = rotate({pos_src.z, 0.0, 0.0}, mtx);
  } catch (...) {
    throw;
  }

  return pos_res;
}

}  // namespace apparent_sun_moon

