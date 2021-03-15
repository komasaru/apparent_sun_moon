#include "nutation.hpp"

namespace apparent_sun_moon {

// 定数
static constexpr double kPi     = atan(1.0) * 4;  // PI
static constexpr double kPi2    = kPi * 2;        // PI * 2
static constexpr double kAs2R   = 4.848136811095359935899141e-6;  // Arcseconds to radians
static constexpr double kTurnas = 1296000.0;      // Arcseconds in a full circle
static constexpr double kU2R    = kAs2R / 1.0e7;  // Units of 0.1 microarcsecond to radians

/*
 * @brief       コンストラクタ
 *
 * @param[in]   Julian Century Number(double)
 * @param[ref]  lunisolar parameter 一覧 (vector<vector<double>>)
 * @param[ref]  planetary parameter 一覧 (vector<vector<double>>)
 */
Nutation::Nutation(
    double t,
    std::vector<std::vector<double>>& dat_ls,
    std::vector<std::vector<double>>& dat_pl) {
  try {
    this->t = t;
    this->dat_ls = dat_ls;
    this->dat_pl = dat_pl;
  } catch (...) {
    throw;
  }
}

/*
 * @brief       計算: nutation
 *
 * @param[ref]  delta-psi(double)
 * @param[ref]  delta-eps(double)
 * @return      true|false
 */
bool Nutation::calc_nutation(double& dpsi, double& deps) {
  double dpsi_ls;  // delta-psi for lunisolar
  double deps_ls;  // delta-eps for lunisolar
  double dpsi_pl;  // delta-psi for planetary
  double deps_pl;  // delta-eps for planetary

  try {
    if (!calc_lunisolar(dpsi_ls, deps_ls)) {
      std::cout << "[ERROR] Could not calculate delta-psi, "
                << "delta-epsilon for lunisolar!" << std::endl;
      return EXIT_FAILURE;
    }
    if (!calc_planetary(dpsi_pl, deps_pl)) {
      std::cout << "[ERROR] Could not calculate delta-psi, "
                << "delta-epsilon for lunisolar!" << std::endl;
      return EXIT_FAILURE;
    }
    dpsi = dpsi_ls + dpsi_pl;
    deps = deps_ls + deps_pl;
  } catch (...) {
    return false;
  }

  return true;
}

// -------------------------------------
// 以下、 private functions
// -------------------------------------

/*
 * @brief       計算: lunisolar
 *
 * @param[ref]  delta-psi(double)
 * @param[ref]  delta-eps(double)
 * @return      true|false
 */
bool Nutation::calc_lunisolar(double& dpsi, double& deps) {
  double l;         // work
  double lp;        // work
  double f;         // work
  double d;         // work
  double om;        // work
  double a;         // work
  double ca;        // work
  double sa;        // work
  double dp = 0.0;  // work
  double de = 0.0;  // work
  int i;            // loop index

  try {
    l  = calc_l_iers2003();
    lp = calc_lp_mhb2000();
    f  = calc_f_iers2003();
    d  = calc_d_mhb2000();
    om = calc_om_iers2003();
    for (i = dat_ls.size() - 1; i >= 0; --i) {
      a = dat_ls[i][0] * l + dat_ls[i][1] * lp + dat_ls[i][2] * f
        + dat_ls[i][3] * d + dat_ls[i][4] * om;
      a = fmod_p(a, kPi2);
      sa = std::sin(a);
      ca = std::cos(a);
      dp += (dat_ls[i][5] + dat_ls[i][6] * t) * sa + dat_ls[i][ 7] * ca;
      de += (dat_ls[i][8] + dat_ls[i][9] * t) * ca + dat_ls[i][10] * sa;
    }
    dpsi = dp * kU2R;
    deps = de * kU2R;
  } catch (...) {
    return false;
  }

  return true;
}

/*
 * @brief       計算: planetary
 *
 * @param[ref]  delta-psi(double)
 * @param[ref]  delta-eps(double)
 * @return      true|false
 */
bool Nutation::calc_planetary(double& dpsi, double& deps) {
  double l;         // work
  double f;         // work
  double d;         // work
  double om;        // work
  double pa;        // work
  double ca;        // work
  double lme;       // work
  double lve;       // work
  double lea;       // work
  double lma;       // work
  double lju;       // work
  double lsa;       // work
  double lur;       // work
  double lne;       // work
  double a;         // work
  double sa;        // work
  double dp = 0.0;  // work
  double de = 0.0;  // work
  int i;            // loop index

  try {
    l   = calc_l_mhb2000();
    f   = calc_f_mhb2000();
    d   = calc_d_mhb2000_2();
    om  = calc_om_mhb2000();
    pa  = calc_pa_iers2003();
    lme = calc_lme_iers2003();
    lve = calc_lve_iers2003();
    lea = calc_lea_iers2003();
    lma = calc_lma_iers2003();
    lju = calc_lju_iers2003();
    lsa = calc_lsa_iers2003();
    lur = calc_lur_iers2003();
    lne = calc_lne_mhb2000();
    for (i = dat_pl.size() - 1; i >= 0; --i) {
      a = dat_pl[i][ 0] * l   + dat_pl[i][ 2] * f   + dat_pl[i][ 3] * d
        + dat_pl[i][ 4] * om  + dat_pl[i][ 5] * lme + dat_pl[i][ 6] * lve
        + dat_pl[i][ 7] * lea + dat_pl[i][ 8] * lma + dat_pl[i][ 9] * lju
        + dat_pl[i][10] * lsa + dat_pl[i][11] * lur + dat_pl[i][12] * lne
        + dat_pl[i][13] * pa;
      a = fmod_p(a, kPi2);
      sa = std::sin(a);
      ca = std::cos(a);
      dp += dat_pl[i][14] * sa + dat_pl[i][15] * ca;
      de += dat_pl[i][16] * sa + dat_pl[i][17] * ca;
    }
    dpsi = dp * kU2R;
    deps = de * kU2R;
  } catch (...) {
    return false;
  }

  return true;
}

/*
 * @brief   計算: Mean anomaly of the Moon (IERS 2003)
 *
 * @param   <none>
 * @return  Mean anomaly of the Moon (IERS 2003)(double)
 */
double Nutation::calc_l_iers2003() {
  double v;

  try {
    v = (    485868.249036  +
        (1717915923.2178    +
        (        31.8792    +
        (         0.051635  +
        (        -0.00024470)
        * t) * t) * t) * t);
    v = fmod_p(v, kTurnas) * kAs2R;
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: Mean anomaly of the Sun (MHB2000)
 *
 * @param   <none>
 * @return  Mean anomaly of the Sun (MHB2000)(double)
 */
double Nutation::calc_lp_mhb2000() {
  double v;

  try {
    v = (  1287104.79305   +
        (129596581.0481    +
        (       -0.5532    +
        (        0.000136  +
        (       -0.00001149)
        * t) * t) * t) * t);
    v = fmod_p(v, kTurnas) * kAs2R;
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: Mean longitude of the Moon minus that of the ascending node (IERS 2003)
 *
 * @param   <none>
 * @return  Mean longitude of the Moon minus that of the ascending node (IERS 2003)(double)
 */
double Nutation::calc_f_iers2003() {
  double v;

  try {
    v = (    335779.526232  +
        (1739527262.8478    +
        (       -12.7512    +
        (        -0.001037  +
        (         0.00000417)
        * t) * t) * t) * t);
    v = fmod_p(v, kTurnas) * kAs2R;
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: Mean elongation of the Moon from the Sun (MHB2000)
 *
 * @param   <none>
 * @return  Mean elongation of the Moon from the Sun (MHB2000)(double)
 */
double Nutation::calc_d_mhb2000() {
  double v;

  try {
    v = (   1072260.70369   +
        (1602961601.2090    +
        (        -6.3706    +
        (         0.006593  +
        (        -0.00003169)
        * t) * t) * t) * t);
    v = fmod_p(v, kTurnas) * kAs2R;
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: Mean longitude of the ascending node of the Moon (IERS 2003)
 *
 * @param   <none>
 * @return  Mean longitude of the ascending node of the Moon (IERS 2003)(double)
 */
double Nutation::calc_om_iers2003() {
  double v;

  try {
    v = (  450160.398036  +
        (-6962890.5431    +
        (       7.4722    +
        (       0.007702  +
        (      -0.00005939)
        * t) * t) * t) * t);
    v = fmod_p(v, kTurnas) * kAs2R;
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: Mean anomaly of the Moon (MHB2000)
 *
 * @param   <none>
 * @return  Mean anomaly of the Moon (MHB2000)(double)
 */
double Nutation::calc_l_mhb2000() {
  double v;

  try {
    v = std::fmod(2.35555598 + 8328.6914269554 * t, kPi2);
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: Mean longitude of the Moon minus that of the ascending node (MHB2000)
 *
 * @param   <none
 * @return  Mean longitude of the Moon minus that of the ascending node (MHB2000)(double)
 */
double Nutation::calc_f_mhb2000() {
  double v;

  try {
    v = std::fmod(1.627905234 + 8433.466158131 * t, kPi2);
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: Mean elongation of the Moon from the Sun (MHB2000)
 *
 * @param   <none>
 * @return  Mean elongation of the Moon from the Sun (MHB2000)(double)
 */
double Nutation::calc_d_mhb2000_2() {
  double v;

  try {
    v = std::fmod(5.198466741 + 7771.3771468121 * t, kPi2);
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: Mean longitude of the ascending node of the Moon (MHB2000)
 *
 * @param   <none>
 * @return  Mean longitude of the ascending node of the Moon (MHB2000)(double)
 */
double Nutation::calc_om_mhb2000() {
  double v;

  try {
    // v < 0 になる可能性がなくもないので、 fmod_p を使用
    v = 2.18243920 - 33.757045 * t;
    v = fmod_p(v, kPi2);
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: General accumulated precession in longitude (IERS 2003)
 *
 * @param   <none>
 * @return  General accumulated precession in longitude (IERS 2003)(double)
 */
double Nutation::calc_pa_iers2003() {
  double v;

  try {
    v = (0.024381750 + 0.00000538691 * t) * t;
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: Mercury longitudes (IERS 2003)
 *
 * @param   <none>
 * @return  Mercury longitudes (IERS 2003)(double)
 */
double Nutation::calc_lme_iers2003() {
  double v;

  try {
    v = std::fmod(4.402608842 + 2608.7903141574 * t, kPi2);
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: Venus longitudes (IERS 2003)
 *
 * @param   <none>
 * @return  Venus longitudes (IERS 2003)(double)
 */
double Nutation::calc_lve_iers2003() {
  double v;

  try {
    v = std::fmod(3.176146697 + 1021.3285546211 * t, kPi2);
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: Earth longitudes (IERS 2003)
 *
 * @param   <none>
 * @return  Earth longitudes (IERS 2003)(double)
 */
double Nutation::calc_lea_iers2003() {
  double v;

  try {
    v = std::fmod(1.753470314 + 628.3075849991 * t, kPi2);
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: Mars longitudes (IERS 2003)
 *
 * @param   <none>
 * @return  Mars longitudes (IERS 2003)(double)
 */
double Nutation::calc_lma_iers2003() {
  double v;

  try {
    v = std::fmod(6.203480913 + 334.0612426700 * t, kPi2);
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: Jupiter longitudes (IERS 2003)
 *
 * @param   <none>
 * @return  Jupiter longitudes (IERS 2003)(double)
 */
double Nutation::calc_lju_iers2003() {
  double v;

  try {
    v = std::fmod(0.599546497 + 52.9690962641 * t, kPi2);
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: Saturn longitudes (IERS 2003)
 *
 * @param   <none>
 * @return  Saturn longitudes (IERS 2003)(double)
 */
double Nutation::calc_lsa_iers2003() {
  double v;

  try {
    v = std::fmod(0.874016757 + 21.3299104960 * t, kPi2);
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: Uranus longitudes (IERS 2003)
 *
 * @param   <none>
 * @return  Uranus longitudes (IERS 2003)(double)
 */
double Nutation::calc_lur_iers2003() {
  double v;

  try {
    v = std::fmod(5.481293872 + 7.4781598567 * t, kPi2);
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief   計算: Neptune longitude (MHB2000)
 *
 * @param   <none>
 * @return  Neptune longitude (MHB2000)(double)
 */
double Nutation::calc_lne_mhb2000() {
  double v;

  try {
    v = std::fmod(5.321159000 + 3.8127774000 * t, kPi2);
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief      正剰余計算
 *             （std::fmod が正剰余に非対応のため）
 *
 * @param[in]  被除数(double)
 * @param[in]  除数(double)
 * @return     計算後の値(double)
 */
double Nutation::fmod_p(double a, double n) {
  try {
    return a - std::floor(a / n) * n;
  } catch (...) {
    throw;
  }
}

}  // namespace apparent_sun_moon

