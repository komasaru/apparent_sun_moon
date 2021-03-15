#include "matrix.hpp"

namespace apparent_sun_moon {

/*
 * @brief       回転行列（x軸中心）
 *
 * @param[in]   回転量 phi(double)
 * @param[ref]  生成後(double[3][3])
 * @param[ref]  生成前(double[3][3])
 * @return      true|false
 */
bool r_x(double phi, double(&mtx)[3][3], const double(&mtx_u)[3][3]) {
  double s;  // sin
  double c;  // cos
  bool ret = true;

  try {
    s = sin(phi);
    c = cos(phi);
    mtx[0][0] =       mtx_u[0][0];
    mtx[0][1] =       mtx_u[0][1];
    mtx[0][2] =       mtx_u[0][2];
    mtx[1][0] =   c * mtx_u[1][0] + s * mtx_u[2][0];
    mtx[1][1] =   c * mtx_u[1][1] + s * mtx_u[2][1];
    mtx[1][2] =   c * mtx_u[1][2] + s * mtx_u[2][2];
    mtx[2][0] = - s * mtx_u[1][0] + c * mtx_u[2][0];
    mtx[2][1] = - s * mtx_u[1][1] + c * mtx_u[2][1];
    mtx[2][2] = - s * mtx_u[1][2] + c * mtx_u[2][2];
  } catch (...) {
    throw;
  }

  return ret;
}

/*
 * @brief       回転行列（y軸中心）
 *
 * @param[in]   回転量 theta(double)
 * @param[ref]  生成後(double[3][3])
 * @param[ref]  生成前(double[3][3])
 * @return      true|false
 */
bool r_y(double theta, double(&mtx)[3][3], const double(&mtx_u)[3][3]) {
  double s;  // sin
  double c;  // cos
  bool ret = true;

  try {
    s = sin(theta);
    c = cos(theta);
    mtx[0][0] = c * mtx_u[0][0] - s * mtx_u[2][0];
    mtx[0][1] = c * mtx_u[0][1] - s * mtx_u[2][1];
    mtx[0][2] = c * mtx_u[0][2] - s * mtx_u[2][2];
    mtx[1][0] =     mtx_u[1][0];
    mtx[1][1] =     mtx_u[1][1];
    mtx[1][2] =     mtx_u[1][2];
    mtx[2][0] = s * mtx_u[0][0] + c * mtx_u[2][0];
    mtx[2][1] = s * mtx_u[0][1] + c * mtx_u[2][1];
    mtx[2][2] = s * mtx_u[0][2] + c * mtx_u[2][2];
  } catch (...) {
    throw;
  }

  return ret;
}

/*
 * @brief       回転行列（z軸中心）
 *
 * @param[in]   回転量 psi(double)
 * @param[ref]  生成後(double[3][3])
 * @param[ref]  生成前(double[3][3])
 * @return      true|false
 */
bool r_z(double psi, double(&mtx)[3][3], const double(&mtx_u)[3][3]) {
  double s;  // sin
  double c;  // cos
  bool ret = true;

  try {
    s = sin(psi);
    c = cos(psi);
    mtx[0][0] =   c * mtx_u[0][0] + s * mtx_u[1][0];
    mtx[0][1] =   c * mtx_u[0][1] + s * mtx_u[1][1];
    mtx[0][2] =   c * mtx_u[0][2] + s * mtx_u[1][2];
    mtx[1][0] = - s * mtx_u[0][0] + c * mtx_u[1][0];
    mtx[1][1] = - s * mtx_u[0][1] + c * mtx_u[1][1];
    mtx[1][2] = - s * mtx_u[0][2] + c * mtx_u[1][2];
    mtx[2][0] =       mtx_u[2][0];
    mtx[2][1] =       mtx_u[2][1];
    mtx[2][2] =       mtx_u[2][2];
  } catch (...) {
    throw;
  }

  return ret;
}

/*
 * @brief       座標回転
 *
 * @param[in]   回転前直交座標(PositionRect)
 * @param[ref]  回転行列(double[3][3])
 * @return      回転後直交座標(PositionRect)
 */
Coord rotate(Coord pos_src, double(&mtx)[3][3]) {
  Coord pos_dst;

  try {
    pos_dst.x = mtx[0][0] * pos_src.x
              + mtx[0][1] * pos_src.y
              + mtx[0][2] * pos_src.z;
    pos_dst.y = mtx[1][0] * pos_src.x
              + mtx[1][1] * pos_src.y
              + mtx[1][2] * pos_src.z;
    pos_dst.z = mtx[2][0] * pos_src.x
              + mtx[2][1] * pos_src.y
              + mtx[2][2] * pos_src.z;
  } catch (...) {
    throw;
  }

  return pos_dst;
}

}  // namespace apparent_sun_moon

