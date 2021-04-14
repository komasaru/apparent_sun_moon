#ifndef APPARENT_SUN_MOON_NUTATION_HPP_
#define APPARENT_SUN_MOON_NUTATION_HPP_

#include "file.hpp"

#include <cmath>
#include <iostream>
#include <vector>

namespace apparent_sun_moon {

class Nutation {
  static std::vector<std::vector<double>> dat_ls;  // data of lunisolar parameters
  static std::vector<std::vector<double>> dat_pl;  // data of planetary parameters
  double t;                                 // Julian Century Number for TT

public:
  Nutation(double t);                       // コンストラクタ
  bool calc_nutation(double&, double&);     // 計算: nutation

private:
  bool calc_lunisolar(double&, double&);    // 計算: lunisolar
  bool calc_planetary(double&, double&);    // 計算: planetary
  double calc_l_iers2003();                 // Mean anomaly of the Moon (IERS 2003)
  double calc_lp_mhb2000();                 // Mean anomaly of the Sun (MHB2000)
  double calc_f_iers2003();                 // Mean longitude of the Moon minus that of the ascending node (IERS 2003)
  double calc_d_mhb2000();                  // Mean elongation of the Moon from the Sun (MHB2000)
  double calc_om_iers2003();                // Mean longitude of the ascending node of the Moon (IERS 2003)
  double calc_l_mhb2000();                  // Mean anomaly of the Moon (MHB2000)
  double calc_f_mhb2000();                  // Mean longitude of the Moon minus that of the ascending node (MHB2000)
  double calc_d_mhb2000_2();                // Mean elongation of the Moon from the Sun (MHB2000)
  double calc_om_mhb2000();                 // Mean longitude of the ascending node of the Moon (MHB2000)
  double calc_pa_iers2003();                // General accumulated precession in longitude (IERS 2003)
  double calc_lme_iers2003();               // Mercury longitudes (IERS 2003)
  double calc_lve_iers2003();               // Venus longitudes (IERS 2003)
  double calc_lea_iers2003();               // Earth longitudes (IERS 2003)
  double calc_lma_iers2003();               // Mars longitudes (IERS 2003)
  double calc_lju_iers2003();               // Jupiter longitudes (IERS 2003)
  double calc_lsa_iers2003();               // Saturn longitudes (IERS 2003)
  double calc_lur_iers2003();               // Uranus longitudes (IERS 2003)
  double calc_lne_mhb2000();                // Neptune longitude (MHB2000)
  double fmod_p(double, double);            // 正剰余計算（std::fmod が非対応のため）
};

}  // namespace apparent_sun_moon

#endif

