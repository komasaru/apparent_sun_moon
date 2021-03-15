#ifndef APPARENT_SUN_MOON_BPN_HPP_
#define APPARENT_SUN_MOON_BPN_HPP_

#include "coord.hpp"
#include "matrix.hpp"
#include "nutation.hpp"
#include "obliquity.hpp"

#include <ctime>
#include <iostream>
#include <iomanip>   // for setprecision

namespace apparent_sun_moon {

class Bpn {
  std::vector<std::vector<double>> dat_ls;  // Parameters of lunisolar
  std::vector<std::vector<double>> dat_pl;  // Parameters of planetary
  double jcn;                    // JCN(T; ユリウス世紀数)
  double eps;                    // 黄道傾斜角
  double r_bias[3][3];           // 回転行列（バイアス）
  double r_bias_prec[3][3];      // 回転行列（バイアス＆歳差）
  double r_bias_prec_nut[3][3];  // 回転行列（バイアス＆歳差＆章動）
  double r_prec[3][3];           // 回転行列（歳差）
  double r_prec_nut[3][3];       // 回転行列（歳差＆章動）
  double r_nut[3][3];            // 回転行列（章動）

public:
  Bpn(double,
      std::vector<std::vector<double>>&,
      std::vector<std::vector<double>>&);     // コンストラクタ
  bool gen_r_bias(double(&)[3][3]);           // 変換行列生成: Bias
  bool gen_r_bias_prec(double(&)[3][3]);      // 変換行列生成: バイアス＆歳差
  bool gen_r_bias_prec_nut(double(&)[3][3]);  // 変換行列生成: バイアス＆歳差＆章動
  bool gen_r_prec(double(&)[3][3]);           // 変換行列生成: 歳差
  bool gen_r_prec_nut(double(&)[3][3]);       // 変換行列生成: 歳差＆章動
  bool gen_r_nut(double(&)[3][3]);            // 変換行列生成: 章動
  Coord apply_bias(Coord);                    // Bias（バイアス） 適用
  Coord apply_bias_prec(Coord);               // Bias（バイアス） & Precession（歳差) 適用
  Coord apply_bias_prec_nut(Coord);           // Bias（バイアス） & Precession（歳差)  & Nutation（章動） 適用
  Coord apply_prec(Coord);                    // Precession（歳差） 適用
  Coord apply_prec_nut(Coord);                // Precession（歳差） & Nutation（章動） 適用
  Coord apply_nut(Coord);                     // Nutation（章動） 適用

private:
  double comp_gamma_bp();                     // 計算: バイアス＆歳差変換行列用 gamma
  double comp_phi_bp();                       // 計算: バイアス＆歳差変換行列用 phi
  double comp_psi_bp();                       // 計算: バイアス＆歳差変換行列用 psi
  double comp_gamma_p();                      // 計算: 歳差変換行列用 gamma
  double comp_phi_p();                        // 計算: 歳差変換行列用 phi
  double comp_psi_p();                        // 計算: 歳差変換行列用 psi

};

}  // namespace apparent_sun_moon

#endif

