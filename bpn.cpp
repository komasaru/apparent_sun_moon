#include "bpn.hpp"

namespace apparent_sun_moon {

// 定数
static constexpr double kPi    = atan(1.0) * 4;  // PI
static constexpr double kPi2   = kPi * 2;        // PI * 2
static constexpr double kAs2R  = kPi / (3600.0 * 180.0);  // arcseconds -> radians
static constexpr double kMas2R = kAs2R / 1000.0;          // millarcsecond -> radian

/*
 * @brief      コンストラクタ
 *
 * @param[in]  JCN (double)
 */
Bpn::Bpn(double jcn) {
  try {
    // JCN
    this->jcn = jcn;
    // 黄道傾斜角計算
    Obliquity o_ob;
    eps = o_ob.calc_ob(jcn);
    // 回転行列生成
    if (!gen_r_bias(         r_bias         )) throw;
    if (!gen_r_bias_prec(    r_bias_prec    )) throw;
    if (!gen_r_bias_prec_nut(r_bias_prec_nut)) throw;
    if (!gen_r_prec(         r_prec         )) throw;
    if (!gen_r_prec_nut(     r_prec_nut     )) throw;
    if (!gen_r_nut(          r_nut          )) throw;
  } catch (...) {
    throw;
  }
}

/*
 * @brief      Bias 変換行列（一般的な理論）生成
 *
 *             赤道座標(J2000.0)の極は ICRS の極に対して12時（x軸のマイナス側）の方
 *             向へ 17.3±0.2 mas、18時（y軸のマイナス側）の方向へ 5.1±0.2 mas ズレ
 *             ているので、変換する。
 *             さらに、平均分点への変換はICRSでの赤経を78±10 mas、天の極を中心に回
 *             転させる。
 *               18時の方向の変換はx軸を-5.1mas回転（以下の A は回転量(rad)）
 *                         + 1     0      0   +
 *                 R1(A) = | 0   cosA   sinA  |
 *                         + 0  -sinA   cosA  +
 *               12時の方向の変換はy軸を-17.3mas回転
 *                         + cosA   0  -sinA  +
 *                 R2(A) = |   0    1     0   |
 *                         + sinA   0   cosA  +
 *               天の極を中心に78.0mas回転
 *                         +  cosA   sinA   0 +
 *                 R3(A) = | -sinA   cosA   0 |
 *                         +    0      0    1 +
 *
 * @param[ref] 回転行列(double[3][3])
 * @return     true|false
 */
bool Bpn::gen_r_bias(double(&r)[3][3]) {
  double r_0[3][3];
  double r_1[3][3];

  try {
    if (!r_x( -5.1 * kMas2R, r_0     )) throw;
    if (!r_y(-17.3 * kMas2R, r_1, r_0)) throw;
    if (!r_z( 78.0 * kMas2R, r  , r_1)) throw;
  } catch (...) {
    return false;
  }

  return true;
}

/*
 * @brief      Bias + Precession 変換行列生成
 *
 *             IAU 2006 (Fukushima-Williams 4-angle formulation) 理論
 *
 * @param[ref] 回転行列(double[3][3])
 * @return     true|false
 */
bool Bpn::gen_r_bias_prec(double(&r)[3][3]) {
  double gamma;
  double phi;
  double psi;
  double r_0[3][3];
  double r_1[3][3];
  double r_2[3][3];

  try {
    gamma = comp_gamma_bp();
    phi   = comp_phi_bp();
    psi   = comp_psi_bp();
    if (!r_z(gamma, r_0     )) throw;
    if (!r_x(  phi, r_1, r_0)) throw;
    if (!r_z( -psi, r_2, r_1)) throw;
    if (!r_x( -eps, r  , r_2)) throw;
  } catch (...) {
    return false;
  }

  return true;
}

/*
 * @brief      Bias + Precession + Nutation 変換行列生成
 *
 *             IAU 2006 (Fukushima-Williams 4-angle formulation) 理論
 *
 * @param[ref] 回転行列(double[3][3])
 * @return     true|false
 */
bool Bpn::gen_r_bias_prec_nut(double(&r)[3][3]) {
  double gamma;
  double phi;
  double psi;
  double fj2;
  double dpsi;
  double deps;
  double r_0[3][3];
  double r_1[3][3];
  double r_2[3][3];

  try {
    // Nutation(delta-psi, delta-eps) 計算
    Nutation o_n(jcn);
    if (!o_n.calc_nutation(dpsi, deps)) {
      std::cout << "[ERROR] Could not calculate delta-psi, "
                << "delta-epsilon!" << std::endl;
      return EXIT_FAILURE;
    }
    // 変換行列生成
    gamma = comp_gamma_bp();
    phi   = comp_phi_bp();
    psi   = comp_psi_bp();
    fj2 = -2.7774e-6 * jcn;
    dpsi += dpsi * (0.4697e-6 + fj2);
    deps += deps * fj2;
    if (!r_z(    gamma, r_0     )) throw;
    if (!r_x(      phi, r_1, r_0)) throw;
    if (!r_z(-psi-dpsi, r_2, r_1)) throw;
    if (!r_x(-eps-deps, r  , r_2)) throw;
  } catch (...) {
    return false;
  }

  return true;
}

/*
 * @brief      precession（歳差）変換行列（J2000.0 用）生成
 *
 *            歳差の変換行列
 *              P(ε , ψ , φ , γ ) = R1(-ε ) * R3(-ψ ) * R1(φ ) * R3(γ )
 *            但し、R1, R2, R3 は x, y, z 軸の回転。
 *                       + 1     0       0   +            +  cosθ   sinθ   0 +
 *              R1(θ) = | 0   cosθ   sinθ | , R3(θ) = | -sinθ   cosθ   0 |
 *                       + 0  -sinθ   cosθ +            +    0       0     1 +
 *                                  + P_11 P_12 P_13 +
 *              P(ε, ψ, φ, γ) = | P_21 P_22 P_23 | とすると、
 *                                  + P_31 P_32 P_33 +
 *              P_11 = cosψ cosγ + sinψ cosφ sinγ
 *              P_12 = cosψ sinγ - sinψ cosφ ̄cosγ
 *              P_13 = -sinψ sinφ
 *              P_21 = cosε sinψ cosγ - (cosε cosψ cosφ + sinε sinφ )sinγ
 *              P_22 = cosε sinψ cosγ + (cosε cosψ cosφ + sinε sinφ )cosγ
 *              P_23 = cosε cosψ sinφ - sinε cosφ
 *              P_31 = sinε sinψ cosγ - (sinε cosψ cosφ - cosε sinφ)sinγ
 *              P_32 = sinε sinψ cosγ + (sinε cosψ cosφ - cosε sinφ)cosγ
 *              P_33 = sinε cosψ sinφ + cosε cosφ
 *
 * @param[ref] 回転行列(double[3][3])
 * @return     true|false
 */
bool Bpn::gen_r_prec(double(&r)[3][3]) {
  double gamma;
  double phi;
  double psi;
  double r_0[3][3];
  double r_1[3][3];
  double r_2[3][3];

  try {
    gamma = comp_gamma_p();
    phi   = comp_phi_p();
    psi   = comp_psi_p();
    if (!r_z(gamma, r_0     )) throw;
    if (!r_x(  phi, r_1, r_0)) throw;
    if (!r_z( -psi, r_2, r_1)) throw;
    if (!r_x( -eps, r  , r_2)) throw;
  } catch (...) {
    return false;
  }

  return true;
}

/*
 * @brief      Precession（歳差） & Nutation（章動） 変換行列生成
 *
 *             IAU 2000A nutation with adjustments to match the IAU 2006 precession.
 *
 * @param[ref] 回転行列(double[3][3])
 * @return     true|false
 */
bool Bpn::gen_r_prec_nut(double(&r)[3][3]) {
  double gamma;
  double phi;
  double psi;
  double fj2;
  double dpsi;
  double deps;
  double r_0[3][3];
  double r_1[3][3];
  double r_2[3][3];

  try {
    // Nutation(delta-psi, delta-eps) 計算
    Nutation o_n(jcn);
    if (!o_n.calc_nutation(dpsi, deps)) {
      std::cout << "[ERROR] Could not calculate delta-psi, "
                << "delta-epsilon!" << std::endl;
      return EXIT_FAILURE;
    }
    // 変換行列生成
    gamma = comp_gamma_p();
    phi   = comp_phi_p();
    psi   = comp_psi_p();
    fj2 = -2.7774e-6 * jcn;
    dpsi += dpsi * (0.4697e-6 + fj2);
    deps += deps * fj2;
    if (!r_z(    gamma, r_0     )) throw;
    if (!r_x(      phi, r_1, r_0)) throw;
    if (!r_z(-psi-dpsi, r_2, r_1)) throw;
    if (!r_x(-eps-deps, r  , r_2)) throw;
  } catch (...) {
    return false;
  }

  return true;
}

/*
 * @brief      nutation（章動）変換行列生成
 *
 *             IAU 2000A nutation with adjustments to match the IAU 2006 precession.
 *
 * @param[ref] 回転行列(double[3][3])
 * @return     true|false
 */
bool Bpn::gen_r_nut(double(&r)[3][3]) {
  double fj2;
  double dpsi;
  double deps;
  double r_0[3][3];
  double r_1[3][3];

  try {
    // Nutation(delta-psi, delta-eps) 計算
    Nutation o_n(jcn);
    if (!o_n.calc_nutation(dpsi, deps)) {
      std::cout << "[ERROR] Could not calculate delta-psi, "
                << "delta-epsilon!" << std::endl;
      return EXIT_FAILURE;
    }
    // 変換行列生成
    fj2 = -2.7774e-6 * jcn;
    dpsi += dpsi * (0.4697e-6 + fj2);
    deps += deps * fj2;
    if (!r_x(      eps, r_0     )) throw;
    if (!r_z(    -dpsi, r_1, r_0)) throw;
    if (!r_x(-eps-deps, r  , r_1)) throw;
  } catch (...) {
    return false;
  }

  return true;
}

/*
 * @brief     Bias（バイアス） 適用
 *
 * @param[in] 適用前直交座標(Coord)
 * @return    適用後直交座標(Coord)
 */
Coord Bpn::apply_bias(Coord pos_src) {
  Coord pos_dst;  // x, y, z

  try {
    pos_dst = rotate(pos_src, r_bias);
  } catch (...) {
    throw;
  }

  return pos_dst;
}

/*
 * @brief     Bias（バイアス） & Precession（歳差） 適用
 *
 * @param[in] 適用前直交座標(Coord)
 * @return    適用後直交座標(Coord)
 */
Coord Bpn::apply_bias_prec(Coord pos_src) {
  Coord pos_dst;  // x, y, z

  try {
    pos_dst = rotate(pos_src, r_bias_prec);
  } catch (...) {
    throw;
  }

  return pos_dst;
}

/*
 * @brief     Bias（バイアス） & Precession（歳差） & Nutation（章動） 適用
 *
 * @param[in] 適用前直交座標(Coord)
 * @return    適用後直交座標(Coord)
 */
Coord Bpn::apply_bias_prec_nut(Coord pos_src) {
  Coord pos_dst;  // x, y, z

  try {
    pos_dst = rotate(pos_src, r_bias_prec_nut);
  } catch (...) {
    throw;
  }

  return pos_dst;
}

/*
 * @brief     Precession（歳差） 適用
 *
 * @param[in] 適用前直交座標(Coord)
 * @return    適用後直交座標(Coord)
 */
Coord Bpn::apply_prec(Coord pos_src) {
  Coord pos_dst;  // x, y, z

  try {
    pos_dst = rotate(pos_src, r_prec);
  } catch (...) {
    throw;
  }

  return pos_dst;
}

/*
 * @brief     Precession（歳差） & Nutation（章動） 適用
 *
 * @param[in] 適用前直交座標(Coord)
 * @return    適用後直交座標(Coord)
 */
Coord Bpn::apply_prec_nut(Coord pos_src) {
  Coord pos_dst;  // x, y, z

  try {
    pos_dst = rotate(pos_src, r_prec_nut);
  } catch (...) {
    throw;
  }

  return pos_dst;
}

/*
 * @brief     Nutation（章動） 適用
 *
 * @param[in] 適用前直交座標(Coord)
 * @return    適用後直交座標(Coord)
 */
Coord Bpn::apply_nut(Coord pos_src) {
  Coord pos_dst;  // x, y, z

  try {
    pos_dst = rotate(pos_src, r_nut);
  } catch (...) {
    throw;
  }

  return pos_dst;
}

// -------------------------------------
// 以下、 private functions
// -------------------------------------

/*
 * @brief      バイアス＆歳差変換行列用 gamma 計算
 *
 * @return     gamma(double)
 */
double Bpn::comp_gamma_bp() {
  double gamma;

  try {
    gamma = (-0.052928    +
            (10.556378    +
            ( 0.4932044   +
            (-0.00031238  +
            (-0.000002788 +
            ( 0.0000000260)
          * jcn) * jcn) * jcn) * jcn) * jcn) * kAs2R;
  } catch (...) {
    throw;
  }

  return gamma;
}

/*
 * @brief      バイアス＆歳差変換行列用 phi 計算
 *
 * @return     phi(double)
 */
double Bpn::comp_phi_bp() {
  double phi;

  try {
    phi = (84381.412819    +
          (  -46.811016    +
          (    0.0511268   +
          (    0.00053289  +
          (   -0.000000440 +
          (   -0.0000000176)
        * jcn) * jcn) * jcn) * jcn) * jcn) * kAs2R;
  } catch (...) {
    throw;
  }

  return phi;
}

/*
 * @brief      バイアス＆歳差変換行列用 psi 計算
 *
 * @return     psi(double)
 */
double Bpn::comp_psi_bp() {
  double psi;

  try {
    psi = (  -0.041775    +
          (5038.481484    +
          (   1.5584175   +
          (  -0.00018522  +
          (  -0.000026452 +
          (  -0.0000000148)
        * jcn) * jcn) * jcn) * jcn) * jcn) * kAs2R;
  } catch (...) {
    throw;
  }

  return psi;
}

/*
 * @brief  歳差変換行列用 gamma 計算
 *
 * @return gamma(double)
 */
double Bpn::comp_gamma_p() {
  double gamma;

  try {
    gamma = ((10.556403     +
             ( 0.4932044    +
             (-0.00031238   +
             (-0.000002788  +
             ( 0.0000000260)
          * jcn) * jcn) * jcn) * jcn) * jcn) * kAs2R;
  } catch (...) {
    throw;
  }

  return gamma;
}

/*
 * @brief      歳差変換行列用 phi 計算
 *
 * @return     phi(double)
 */
double Bpn::comp_phi_p() {
  double phi;

  try {
    phi = (84381.406000    +
          (  -46.811015    +
          (    0.0511269   +
          (    0.00053289  +
          (   -0.000000440 +
          (   -0.0000000176)
        * jcn) * jcn) * jcn) * jcn) * jcn) * kAs2R;
  } catch (...) {
    throw;
  }

  return phi;
}

/*
 * @brief      歳差変換行列用 psi 計算
 *
 * @return     psi(double)
 */
double Bpn::comp_psi_p() {
  double psi;

  try {
    psi = (( 5038.481507    +
           (    1.5584176   +
           (   -0.00018522  +
           (   -0.000026452 +
           (   -0.0000000148)
        * jcn) * jcn) * jcn) * jcn) * jcn) * kAs2R;
  } catch (...) {
    throw;
  }

  return psi;
}

}  // namespace apparent_sun_moon

