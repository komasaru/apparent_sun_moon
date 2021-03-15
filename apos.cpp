#include "apos.hpp"

namespace apparent_sun_moon {

// 定数
static constexpr unsigned long int kC = 299792458;      // 光速 (m/s)
static constexpr unsigned int kDaySec = 86400;          // 1日の秒数(s)
static constexpr double           kPi = atan(1.0) * 4;  // 円周率

/*
 * @brief       コンストラクタ
 *
 * @param[in]   UTC (timespec)
 * @param[ref]  うるう秒一覧 (vector<vector<string>>)
 * @param[ref]  DUT1 一覧 (vector<vector<string>>)
 * @param[ref]  lunisolar parameter 一覧 (vector<vector<double>>)
 * @param[ref]  planetary parameter 一覧 (vector<vector<double>>)
 */
Apos::Apos(
    struct timespec ts,
    std::vector<std::vector<std::string>>& l_ls,
    std::vector<std::vector<std::string>>& l_dut,
    std::vector<std::vector<double>>& dat_ls,
    std::vector<std::vector<double>>& dat_pl) {
  try {
    this->utc    = ts;
    this->l_ls   = l_ls;
    this->l_dut  = l_dut;
    this->dat_ls = dat_ls;
    this->dat_pl = dat_pl;
    Time t_utc(utc, l_ls, l_dut);
    this->tdb = t_utc.calc_tdb();
    Time t_tdb(tdb, l_ls, l_dut);
    this->jd  = t_tdb.calc_jd();
    this->jcn = t_tdb.calc_t();
    calc_val_t2();  // 時刻 t2(TDB) における各種値の計算
  } catch (...) {
    throw;
  }
}

/*
 * @brief   視位置計算: 太陽
 *
 * @param   <none>
 * @return  視位置 (Position)
 */
Position Apos::sun() {
  double   t1_jd;
  Coord    v_21;         // 地球重心(t2)から太陽(t1)への方向ベクトル
  Coord    v_dd;         // 光行差補正後ベクトル
  Coord    pos_sun;      // 太陽位置（直交座標）
  Coord    pos_sun_bpn;  // 太陽位置（直交座標）（バイアス・歳差・章動適用後）
  Coord    eq_pol;       // 赤道極座標
  Coord    ec_rect;      // 黄道直交座標
  Coord    ec_pol;       // 黄道極座標
  Position pos;

  try {
    // === 太陽が光を発した時刻 t1(JD) の計算
    t1_jd = calc_t1(11);
    // === 時刻 t1 における各種値の計算
    calc_val_t1(t1_jd);
    // === 時刻 t2 における地球重心から時刻 t1 における太陽への方向ベクトルの計算
    v_21 = calc_unit_vector(p_e[1], p_s[0]);
    // === GCRS 座標系: 光行差の補正（方向ベクトルの Lorentz 変換）
    v_dd = conv_lorentz(v_21);
    pos_sun = calc_pos(v_dd, d_e_s);
    // === 瞬時の真座標系: GCRS への バイアス・歳差・章動の適用
    Bpn o_bpn(jcn, dat_ls, dat_pl);
    pos_sun_bpn = o_bpn.apply_bias_prec_nut(pos_sun);
    // === 黄道傾斜角
    Obliquity o_ob;
    eps = o_ob.calc_ob(jcn);
    //# === 座標変換
    Convert o_cv(eps);
    eq_pol  = o_cv.rect2pol(pos_sun_bpn);
    ec_rect = o_cv.rect_eq2ec(pos_sun_bpn);
    ec_pol  = o_cv.rect2pol(ec_rect);
    pos.alpha  = eq_pol.x;
    pos.delta  = eq_pol.y;
    pos.d_eq   = eq_pol.z;
    pos.lambda = ec_pol.x;
    pos.beta   = ec_pol.y;
    pos.d_ec   = ec_pol.z;
    // === 視半径／（地平）視差計算
    pos.a_radius = asin(r_s / (eq_pol.z * au)) * 180.0 / kPi * 3600.0;
    pos.parallax = asin(r_e / (eq_pol.z * au)) * 180.0 / kPi * 3600.0;
  } catch (...) {
    throw;
  }

  return pos;
}

/*
 * @brief   視位置計算: 月
 *
 * @param   <none>
 * @return  視位置 (Position)
 */
Position Apos::moon() {
  double   t1_jd;
  Coord    v_21;          // 地球重心(t2)から太陽(t1)への方向ベクトル
  Coord    v_dd;          // 光行差補正後ベクトル
  Coord    pos_moon;      // 月位置（直交座標）
  Coord    pos_moon_bpn;  // 月位置（直交座標）（バイアス・歳差・章動適用後）
  Coord    eq_pol;        // 赤道極座標
  Coord    ec_rect;       // 黄道直交座標
  Coord    ec_pol;        // 黄道極座標
  Position pos;

  try {
    // === 月が光を発した時刻 t1(JD) の計算
    t1_jd = calc_t1(10);
    // === 時刻 t1 における各種値の計算
    calc_val_t1(t1_jd);
    // === 時刻 t2 における地球重心から時刻 t1 における月への方向ベクトルの計算
    v_21 = calc_unit_vector(p_e[1], p_m[0]);
    // === GCRS 座標系: 光行差の補正（方向ベクトルの Lorentz 変換）
    v_dd = conv_lorentz(v_21);
    pos_moon = calc_pos(v_dd, d_e_m);
    // === 瞬時の真座標系: GCRS への バイアス・歳差・章動の適用
    Bpn o_bpn(jcn, dat_ls, dat_pl);
    pos_moon_bpn = o_bpn.apply_bias_prec_nut(pos_moon);
    // === 黄道傾斜角
    Obliquity o_ob;
    eps = o_ob.calc_ob(jcn);
    //# === 座標変換
    Convert o_cv(eps);
    eq_pol  = o_cv.rect2pol(pos_moon_bpn);
    ec_rect = o_cv.rect_eq2ec(pos_moon_bpn);
    ec_pol  = o_cv.rect2pol(ec_rect);
    pos.alpha  = eq_pol.x;
    pos.delta  = eq_pol.y;
    pos.d_eq   = eq_pol.z;
    pos.lambda = ec_pol.x;
    pos.beta   = ec_pol.y;
    pos.d_ec   = ec_pol.z;
    // === 視半径／（地平）視差計算
    pos.a_radius = asin(r_m / (eq_pol.z * au)) * 180.0 / kPi * 3600.0;
    pos.parallax = asin(r_e / (eq_pol.z * au)) * 180.0 / kPi * 3600.0;
  } catch (...) {
    throw;
  }

  return pos;
}

// -------------------------------------
// 以下、 private functions
// -------------------------------------
//

/*
 * @brief   時刻 t2 におけるの各種値の計算
 *          (3:地球, 10:月, 11:太陽)
 *          (天体番号 12: 太陽系重心)
 *
 * @param   <none>
 * @return  <none>
 */
void Apos::calc_val_t2() {
  try {
    // バイナリファイル読み込み
    Jpl o_jpl(jd);
    o_jpl.read_bin();
    au = o_jpl.au;
    // ICRS 座標(3: 地球)
    o_jpl.calc_pv(3, 12);
    p_e[1].x = o_jpl.pos[0];
    p_e[1].y = o_jpl.pos[1];
    p_e[1].z = o_jpl.pos[2];
    v_e[1].x = o_jpl.vel[0];
    v_e[1].y = o_jpl.vel[1];
    v_e[1].z = o_jpl.vel[2];
    // ICRS 座標(10: 月)
    o_jpl.calc_pv(10, 12);
    p_m[1].x = o_jpl.pos[0];
    p_m[1].y = o_jpl.pos[1];
    p_m[1].z = o_jpl.pos[2];
    v_m[1].x = o_jpl.vel[0];
    v_m[1].y = o_jpl.vel[1];
    v_m[1].z = o_jpl.vel[2];
    // ICRS 座標(11: 太陽)
    o_jpl.calc_pv(11, 12);
    p_s[1].x = o_jpl.pos[0];
    p_s[1].y = o_jpl.pos[1];
    p_s[1].z = o_jpl.pos[2];
    v_s[1].x = o_jpl.vel[0];
    v_s[1].y = o_jpl.vel[1];
    v_s[1].z = o_jpl.vel[2];
    // 時刻 t2 における地球と太陽・月の距離
    d_e_s = calc_dist(p_e[1], p_s[1]);
    d_e_m = calc_dist(p_e[1], p_m[1]);
    // 太陽／月／地球の半径取得
    r_s = get_cval(o_jpl.cnams, o_jpl.cvals, "ASUN");
    r_m = get_cval(o_jpl.cnams, o_jpl.cvals, "AM");
    r_e = get_cval(o_jpl.cnams, o_jpl.cvals, "RE");
  } catch (...) {
    throw;
  }
}

/*
 * @brief      時刻 t1 におけるの各種値の計算
 *             (3:地球, 10:月, 11:太陽)
 *             (天体番号 12: 太陽系重心)
 *
 * @param[in]  Julian Day (double)
 * @return     <none>
 */
void Apos::calc_val_t1(double t1) {
  try {
    // バイナリファイル読み込み
    Jpl o_jpl(t1);
    o_jpl.read_bin();
    // ICRS 座標(3: 地球)
    o_jpl.calc_pv(3, 12);
    p_e[0].x = o_jpl.pos[0];
    p_e[0].y = o_jpl.pos[1];
    p_e[0].z = o_jpl.pos[2];
    v_e[0].x = o_jpl.vel[0];
    v_e[0].y = o_jpl.vel[1];
    v_e[0].z = o_jpl.vel[2];
    // ICRS 座標(10: 月)
    o_jpl.calc_pv(10, 12);
    p_m[0].x = o_jpl.pos[0];
    p_m[0].y = o_jpl.pos[1];
    p_m[0].z = o_jpl.pos[2];
    v_m[0].x = o_jpl.vel[0];
    v_m[0].y = o_jpl.vel[1];
    v_m[0].z = o_jpl.vel[2];
    // ICRS 座標(11: 太陽)
    o_jpl.calc_pv(11, 12);
    p_s[0].x = o_jpl.pos[0];
    p_s[0].y = o_jpl.pos[1];
    p_s[0].z = o_jpl.pos[2];
    v_s[0].x = o_jpl.vel[0];
    v_s[0].y = o_jpl.vel[1];
    v_s[0].z = o_jpl.vel[2];
  } catch (...) {
    throw;
  }
}

/*
 * @brief      2天体感の距離計算
 *
 * @param[in]  天体1位置 (Coord)
 * @param[in]  天体2位置 (Coord)
 * @return     距離 (double)
 */
double Apos::calc_dist(Coord p_1, Coord p_2) {
  double d;

  try {
    d = sqrt((p_2.x - p_1.x) * (p_2.x - p_1.x)
           + (p_2.y - p_1.y) * (p_2.y - p_1.y)
           + (p_2.z - p_1.z) * (p_2.z - p_1.z));
  } catch (...) {
    throw;
  }

  return d;
}

/*
 * @brief       CVAL 取得
 *
 * @param[ref]  CNAM 一覧 (vector<string>)
 * @param[ref]  CVAL 一覧 (vector<double>)
 * @param[in]   対象 CNAM (string)
 * @return      対象 CVAL (double)
 */
double Apos::get_cval(
    std::vector<std::string>& cnams, std::vector<double>& cvals,
    std::string cnam) {
  double cval;
  unsigned int idx = 0;

  try {
    for (auto a: cnams) {
      if (a == cnam) { break; }
      ++idx;
    }
    cval = cvals[idx];
  } catch (...) {
    throw;
  }

  return cval;
}

/*
 * @brief       計算: 基準天体が光を発した時刻 t1（太陽・月用）
 *              * 計算式： c * (t2 - t1) = r12  (但し、 c: 光の速度。 Newton 法で近似）
 *              * 太陽・月専用なので、太陽・木星・土星・天王星・海王星の重力場による
 *                光の曲がりは非考慮。
 *
 * @param[in]   基準天体番号 (unsigned int)
 * @return      時刻(Julian Day) (timespec)
 */
double Apos::calc_t1(unsigned int target) {
  double       t1;
  double       t2;
  Coord        p_1;
  Coord        v_1;
  Coord        r_12;
  double       d_12;
  double       df;
  double       df_wk;
  unsigned int m;

  try {
    t1 = jd;
    t2 = t1;
    if (target == 10) {
      // 月
      p_1.x = p_m[1].x;
      p_1.y = p_m[1].y;
      p_1.z = p_m[1].z;
      v_1.x = v_m[1].x;
      v_1.y = v_m[1].y;
      v_1.z = v_m[1].z;
    } else if (target == 11) {
      // 太陽
      p_1.x = p_s[1].x;
      p_1.y = p_s[1].y;
      p_1.z = p_s[1].z;
      v_1.x = v_s[1].x;
      v_1.y = v_s[1].y;
      v_1.z = v_s[1].z;
    } else {
      // その他は、取り急ぎ 0.0 を返却
      return 0.0;
    }
    df = 1.0;
    m  = 0;
    while (df > 1.0e-10) {
      r_12.x = p_1.x - p_e[1].x;
      r_12.y = p_1.y - p_e[1].y;
      r_12.z = p_1.z - p_e[1].z;
      d_12 = calc_dist(p_1, p_e[1]);
      df = (kC * kDaySec / au) * (t2 - t1) - d_12;
      df_wk  = r_12.x * v_1.x + r_12.y * v_1.y + r_12.z * v_1.z;
      df /= (kC * kDaySec / (au * 1000.0)) + df_wk / d_12;
      t1 += df;
      ++m;
      if (m > 10) { throw "[ERROR] Newton method error!"; }
      Jpl o_jpl(t1);
      o_jpl.read_bin();
      o_jpl.calc_pv(target, 12);
      p_1.x = o_jpl.pos[0];
      p_1.y = o_jpl.pos[1];
      p_1.z = o_jpl.pos[2];
      v_1.x = o_jpl.vel[0];
      v_1.y = o_jpl.vel[1];
      v_1.z = o_jpl.vel[2];
    }
  } catch (...) {
    throw;
  }

  return t1;
}

/*
 * @brief      計算: 天体Aから見た天体Bの方向ベクトル（太陽・月専用）
 *             * 太陽・月専用なので、太陽・木星・土星・天王星・海王星の重力場による
 *               光の曲がりは非考慮。
 *
 * @param[in]  位置ベクトル(天体A) (Coord)
 * @param[in]  位置ベクトル(天体B) (Coord)
 * @return     方向(単位)ベクトル (Coord)
 */
Coord Apos::calc_unit_vector(Coord pos_a, Coord pos_b) {
  double w;
  Coord  vec;

  try {
    w = calc_dist(pos_a, pos_b);
    vec.x = pos_b.x - pos_a.x;
    vec.y = pos_b.y - pos_a.y;
    vec.z = pos_b.z - pos_a.z;
    if (w != 0.0) {
      vec.x /= w;
      vec.y /= w;
      vec.z /= w;
    }
  } catch (...) {
    throw;
  }

  return vec;
}

/*
 * @brief      光行差の補正（方向ベクトルの Lorentz 変換）
 *             * vec_dd = f * vec_d + (1 + g / (1 + f)) * vec_v
 *               但し、 f = vec_v * vec_d  (ベクトル内積)
 *                      g = sqrt(1 - v^2)  (v: 速度)
 *
 * @param[in]  方向（単位）ベクトル (Coord)
 * @return     補正後ベクトル (Coord)
 */
Coord Apos::conv_lorentz(Coord vec_d) {
  Coord  vec_v;
  Coord  vec_dd_1;
  Coord  vec_dd_2;
  double g;
  double f;
  Coord  vec_dd;

  try {
    vec_v.x = (v_e[1].x / kDaySec) / (kC / (au * 1000.0));
    vec_v.y = (v_e[1].y / kDaySec) / (kC / (au * 1000.0));
    vec_v.z = (v_e[1].z / kDaySec) / (kC / (au * 1000.0));
    g = inner_prod(vec_v, vec_d);
    f = sqrt(1.0 - calc_vel(vec_v));
    vec_dd_1.x = vec_d.x * f;
    vec_dd_1.y = vec_d.y * f;
    vec_dd_1.z = vec_d.z * f;
    vec_dd_2.x = (1.0 + g / (1.0 + f)) * vec_v.x;
    vec_dd_2.y = (1.0 + g / (1.0 + f)) * vec_v.y;
    vec_dd_2.z = (1.0 + g / (1.0 + f)) * vec_v.z;
    vec_dd.x = vec_dd_1.x + vec_dd_2.x;
    vec_dd.y = vec_dd_1.y + vec_dd_2.y;
    vec_dd.z = vec_dd_1.z + vec_dd_2.z;
    vec_dd.x /= 1.0 + g;
    vec_dd.y /= 1.0 + g;
    vec_dd.z /= 1.0 + g;
  } catch (...) {
    throw;
  }

  return vec_dd;
}

/*
 * @brief      ベクトルの内積
 *
 * @param[in]  ベクトル a (Coord)
 * @param[in]  ベクトル b (Coord)
 * @return     内積の値 (double)
 */
double Apos::inner_prod(Coord a, Coord b) {
  double w;

  try {
    w = a.x * b.x + a.y * b.y + a.z * b.z;
  } catch (...) {
    throw;
  }

  return w;
}

/*
 * @brief      計算: 天体の速度ベクトルから実際の速度
 *
 * @param[in]  ベクトル (Coord)
 * @return     速度 (double)
 */
double Apos::calc_vel(Coord vec) {
  double v;

  try {
    v = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
  } catch (...) {
    throw;
  }

  return v;
}

/*
 * @brief      計算: 単位（方向）ベクトルと距離から位置ベクトル
 *
 * @param[in]  単位（方向）ベクトル (Coord)
 * @param[in]  距離 (double)
 * @return     位置ベクトル (Coord)
 */
Coord Apos::calc_pos(Coord d, double r) {
  Coord pos;

  try {
    pos.x = d.x * r;
    pos.y = d.y * r;
    pos.z = d.z * r;
  } catch (...) {
    throw;
  }

  return pos;
}

}  // namespace apparent_sun_moon

