#include "obliquity.hpp"

namespace apparent_sun_moon {

// 定数
static constexpr double kAs2R = 4.848136811095359935899141e-6;  // Arcseconds to radians

/*
 * @brief      黄道傾斜角 計算
 *
 * @param[in]  T (ユリウス世紀数) (double)
 * @return     EPS (黄道傾斜角) (double)
 */
double Obliquity::calc_ob(double t) {
  try {
    return (84381.406
         + (  -46.836769
         + (   -0.0001831
         + (    0.00200340
         + (   -5.76e-7
         + (   -4.34e-8)
         * t) * t) * t) * t) * t) * kAs2R;
  } catch (...) {
    throw;
  }
}

}  // namespace apparent_sun_moon

