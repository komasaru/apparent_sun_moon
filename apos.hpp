#ifndef APPARENT_SUN_MOON_APOS_HPP_
#define APPARENT_SUN_MOON_APOS_HPP_

#include "bpn.hpp"
#include "convert.hpp"
#include "jpl.hpp"
#include "obliquity.hpp"
#include "position.hpp"
#include "time.hpp"

#include <ctime>
#include <iostream>  // for cout etc.

namespace apparent_sun_moon {

// t1 は、基準天体が光を発した時刻、
// t2 は、対象天体に光が到達した時刻
class Apos {
  std::vector<std::vector<std::string>> l_ls;    // List of Leap Second
  std::vector<std::vector<std::string>> l_dut;   // List of DUT1
  std::vector<std::vector<double>>      dat_ls;  // Parameters of lunisolar
  std::vector<std::vector<double>>      dat_pl;  // Parameters of planetary
  struct timespec utc;  // timespec of UTC (of t2)
  double jcn;           // ユリウス世紀数(of TDB)
  double au;            // AU(バイナリデータ)
  Coord  p_e[2];        // t1, t2 における位置(ICRS座標;  3 (地球))
  Coord  v_e[2];        // t1, t2 における速度(ICRS座標;  3 (地球))
  Coord  p_m[2];        // t1, t2 における位置(ICRS座標; 10 (月)  )
  Coord  v_m[2];        // t1, t2 における速度(ICRS座標; 10 (月)  )
  Coord  p_s[2];        // t1, t2 における位置(ICRS座標; 11 (太陽))
  Coord  v_s[2];        // t1, t2 における速度(ICRS座標; 11 (太陽))
  double d_e_m;         // t2 における地球との距離(月)
  double d_e_s;         // t2 における地球との距離(太陽)
  double r_e;           // 半径(地球)
  double r_m;           // 半径(月)
  double r_s;           // 半径(太陽)
  double eps;           // 黄道傾斜角

public:
  struct timespec tdb;     // timespec of TDB (of t2)
  double          jd;      // Julian Day for TDB (of t2)

  Apos(struct timespec);   // コンストラクタ
  Position sun();          // 視位置計算: 太陽
  Position moon();         // 視位置計算: 月

private:
  double calc_dist(Coord, Coord);   // 2点体感の距離計算
  double get_cval(
             std::vector<std::string>&, std::vector<double>&,
             std::string);          // CVAL 取得
  void   calc_val_t2();             // 計算: 時刻 t2 におけるの各種値
  void   calc_val_t1(double);       // 計算: 時刻 t1 におけるの各種値
  double calc_t1(unsigned int);     // 計算: 基準天体が光を発した時刻(JD) t1（太陽・月用）
  Coord  calc_unit_vector(Coord, Coord);
                                    // 計算: 天体Aから見た天体Bの方向ベクトル（太陽・月専用）
  Coord  conv_lorentz(Coord);       // 計算: GCRS 座標系: 光行差の補正(方向ベクトルの Lorentz 変換)
  double inner_prod(Coord, Coord);  // 計算: ベクトルの内積
  double calc_vel(Coord);           // 計算: 天体の速度ベクトルから実際の速度
  Coord  calc_pos(Coord, double);   // 計算: 単位（方向）ベクトルと距離から位置ベクトル
};

}  // namespace apparent_sun_moon

#endif

