#include "delta_t.hpp"

namespace apparent_sun_moon {

/*
 * @brief     ΔT (year < -500)
 *
 * @param[in] 計算用西暦年 (int)
 * @return    ΔT (double)
 */
double calc_dlt_t_bf_m500(double y) {
  double t;
  double dlt_t;

  try {
    t = (y - 1820) / 100.0;
    dlt_t = -20 + 32 * t * t;
  } catch (...) {
    throw;
  }

  return dlt_t;
}

/*
 * @brief     ΔT (-500 <= year && year <   500)
 *
 * @param[in] 計算用西暦年 (int)
 * @return    ΔT (double)
 */
double calc_dlt_t_bf_0500(double y) {
  double t;
  double dlt_t;

  try {
    t = y / 100.0;
    dlt_t = 10583.6         +
           (-1014.41        +
           (   33.78311     +
           (   -5.952053    +
           (   -0.1798452   +
           (    0.022174192 +
           (    0.0090316521)
           * t) * t) * t) * t) * t) * t;
  } catch (...) {
    throw;
  }

  return dlt_t;
}

/*
 * @brief     ΔT ( 500 <= year && year <  1600)
 *
 * @param[in] 計算用西暦年 (int)
 * @return    ΔT (double)
 */
double calc_dlt_t_bf_1600(double y) {
  double t;
  double dlt_t;

  try {
    t = (y - 1000) / 100.0;
    dlt_t = 1574.2         +
           (-556.01        +
           (  71.23472     +
           (   0.319781    +
           (  -0.8503463   +
           (  -0.005050998 +
           (   0.0083572073)
           * t) * t) * t) * t) * t) * t;
  } catch (...) {
    throw;
  }

  return dlt_t;
}

/*
 * @brief     ΔT (1600 <= year && year <  1700)
 *
 * @param[in] 計算用西暦年 (int)
 * @return    ΔT (double)
 */
double calc_dlt_t_bf_1700(double y) {
  double t;
  double dlt_t;

  try {
    t = y - 1600;
    dlt_t = 120           +
           ( -0.9808      +
           ( -0.01532     +
           (  1.0 / 7129.0)
           * t) * t) * t;
  } catch (...) {
    throw;
  }

  return dlt_t;
}

/*
 * @brief     ΔT (1700 <= year && year <  1800)
 *
 * @param[in] 計算用西暦年 (int)
 * @return    ΔT (double)
 */
double calc_dlt_t_bf_1800(double y) {
  double t;
  double dlt_t;

  try {
    t = y - 1700;
    dlt_t =  8.83           +
           ( 0.1603         +
           (-0.0059285      +
           ( 0.00013336     +
           (-1.0 / 1174000.0)
           * t) * t) * t) * t;
  } catch (...) {
    throw;
  }

  return dlt_t;
}

/*
 * @brief     ΔT (1800 <= year && year <  1860)
 *
 * @param[in] 計算用西暦年 (int)
 * @return    ΔT (double)
 */
double calc_dlt_t_bf_1860(double y) {
  double t;
  double dlt_t;

  try {
    t = y - 1800;
    dlt_t = 13.72          +
           (-0.332447      +
           ( 0.0068612     +
           ( 0.0041116     +
           (-0.00037436    +
           ( 0.0000121272  +
           (-0.0000001699  +
           ( 0.000000000875)
           * t) * t) * t) * t) * t) * t) * t;
  } catch (...) {
    throw;
  }

  return dlt_t;
}

/*
 * @brief     ΔT (1860 <= year && year <  1900)
 *
 * @param[in] 計算用西暦年 (int)
 * @return    ΔT (double)
 */
double calc_dlt_t_bf_1900(double y) {
  double t;
  double dlt_t;

  try {
    t = y - 1860;
    dlt_t =  7.62          +
           ( 0.5737        +
           (-0.251754      +
           ( 0.01680668    +
           (-0.0004473624  +
           ( 1.0 / 233174.0)
           * t) * t) * t) * t) * t;
  } catch (...) {
    throw;
  }

  return dlt_t;
}

/*
 * @brief     ΔT (1900 <= year && year <  1920)
 *
 * @param[in] 計算用西暦年 (int)
 * @return    ΔT (double)
 */
double calc_dlt_t_bf_1920(double y) {
  double t;
  double dlt_t;

  try {
    t = y - 1900;
    dlt_t = -2.79      +
           ( 1.494119  +
           (-0.0598939 +
           ( 0.0061966 +
           (-0.000197  )
           * t) * t) * t) * t;
  } catch (...) {
    throw;
  }

  return dlt_t;
}

/*
 * @brief     ΔT (1920 <= year && year <  1941)
 *
 * @param[in] 計算用西暦年 (int)
 * @return    ΔT (double)
 */
double calc_dlt_t_bf_1941(double y) {
  double t;
  double dlt_t;

  try {
    t = y - 1920;
    dlt_t = 21.20     +
           ( 0.84493  +
           (-0.076100 +
           ( 0.0020936)
           * t) * t) * t;
  } catch (...) {
    throw;
  }

  return dlt_t;
}

/*
 * @brief     ΔT (1941 <= year && year <  1961)
 *
 * @param[in] 計算用西暦年 (int)
 * @return    ΔT (double)
 */
double calc_dlt_t_bf_1961(double y) {
  double t;
  double dlt_t;

  try {
    t = y - 1950;
    dlt_t = 29.07      +
           ( 0.407     +
           (-1 / 233.0 +
           ( 1 / 2547.0)
           * t) * t) * t;
  } catch (...) {
    throw;
  }

  return dlt_t;
}

/*
 * @brief     ΔT (1961 <= year && year <  1986)
 *
 * @param[in] 計算用西暦年 (int)
 * @return    ΔT (double)
 */
double calc_dlt_t_bf_1986(double y) {
  double t;
  double dlt_t;

  try {
    t = y - 1975;
    dlt_t = 45.45      +
           ( 1.067     +
           (-1 / 260.0 +
           (-1 / 718.0)
           * t) * t) * t;
  } catch (...) {
    throw;
  }

  return dlt_t;
}

/*
 * @brief     ΔT (1986 <= year && year <  2005)
 *
 * @param[in] 計算用西暦年 (int)
 * @return    ΔT (double)
 */
double calc_dlt_t_bf_2005(double y) {
  double t;
  double dlt_t;

  try {
    t = y - 2000;
    dlt_t = 63.86         +
           ( 0.3345       +
           (-0.060374     +
           ( 0.0017275    +
           ( 0.000651814  +
           ( 0.00002373599)
           * t) * t) * t) * t) * t;
  } catch (...) {
    throw;
  }

  return dlt_t;
}

/*
 * @brief     ΔT (2005 <= year && year <  2050)
 *
 * @param[in] 計算用西暦年 (int)
 * @return    ΔT (double)
 */
double calc_dlt_t_bf_2050(double y) {
  double t;
  double dlt_t;

  try {
    t = y - 2000;
    dlt_t = 62.92    +
           ( 0.32217 +
           ( 0.005589)
           * t) * t;
  } catch (...) {
    throw;
  }

  return dlt_t;
}

/*
 * @brief     ΔT (2050 <= year && year <= 2150)
 *
 * @param[in] 計算用西暦年 (int)
 * @return    ΔT (double)
 */
double calc_dlt_t_to_2150(double y) {
  double t;
  double dlt_t;

  try {
    t = (y - 1820) / 100.0;
    dlt_t = -20
          +  32 * t * t
          -   0.5628 * (2150 - y);
  } catch (...) {
    throw;
  }

  return dlt_t;
}

/*
 * @brief     ΔT (2150 < year)
 *
 * @param[in] 計算用西暦年 (int)
 * @return    ΔT (double)
 */
double calc_dlt_t_af_2150(double y) {
  double t;
  double dlt_t;

  try {
    t = (y - 1820) / 100.0;
    dlt_t = -20 + 32 * t * t;
  } catch (...) {
    throw;
  }

  return dlt_t;
}

}  // namespace apparent_sun_moon

