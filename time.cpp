#include "time.hpp"

namespace apparent_sun_moon{

// 定数
static constexpr int    kJstOffset    = 9;                // JST offset from UTC
static constexpr int    kSecHour      = 3600;             // Seconds in a hour
static constexpr int    kSecDay       = 86400;            // Seconds in a day
static constexpr int    kJ2000        = 2451545;          // Julian Day of 2000-01-01 12:00:00
static constexpr double kJy           = 365.25;           // 1 Julian Year
static constexpr double kTtTai        = 32.184;           // TT - TAI
static constexpr double kLG           = 6.969290134e-10;  // for TCG
static constexpr double kLB           = 1.550519768e-8;   // for TCG, TDB
static constexpr double kT0           = 2443144.5003725;  // for TCG, TDB, TCB
static constexpr double kTdb0         = -6.55e-5;         // for TDB

/*
 * @brief      変換: JST -> UTC
 *
 * @param[in]  JST (timespec)
 * @return     UTC (timespec)
 */
struct timespec jst2utc(struct timespec ts_jst) {
  struct timespec ts;

  try {
    ts.tv_sec  = ts_jst.tv_sec - kJstOffset * kSecHour;
    ts.tv_nsec = ts_jst.tv_nsec;
  } catch (...) {
    throw;
  }

  return ts;
}

/*
 * @brief      日時文字列生成
 *
 * @param[in]  日時 (timespec)
 * @return     日時文字列 (string)
 */
std::string gen_time_str(struct timespec ts) {
  struct tm t;
  std::stringstream ss;
  std::string str_tm;

  try {
    localtime_r(&ts.tv_sec, &t);
    ss << std::setfill('0')
       << std::setw(4) << t.tm_year + 1900 << "-"
       << std::setw(2) << t.tm_mon + 1     << "-"
       << std::setw(2) << t.tm_mday        << " "
       << std::setw(2) << t.tm_hour        << ":"
       << std::setw(2) << t.tm_min         << ":"
       << std::setw(2) << t.tm_sec         << "."
       << std::setw(3) << ts.tv_nsec / 1000000;
    return ss.str();
  } catch (...) {
    throw;
  }
}

// static メンバ変数の初期化
std::vector<std::vector<std::string>> Time::l_ls  = {};  // List of Leap Second
std::vector<std::vector<std::string>> Time::l_dut = {};  // List of DUT1

/*
 * @brief      コンストラクタ
 *
 * @param[in]  UTC(timespec)
 */
Time::Time(struct timespec ts) {
  try {
    // うるう秒, DUT1 一覧、
    if (l_ls.size() == 0 || l_dut.size() == 0) {
      l_ls.reserve(50);    // 予めメモリ確保
      l_dut.reserve(250);  // 予めメモリ確保
      File o_f;
      if (!o_f.get_leap_sec_list(l_ls)) throw;
      if (!o_f.get_dut1_list(l_dut))    throw;
    }
    // その他の初期設定
    this->ts      = ts;
    this->ts_tai  = {};
    this->ts_ut1  = {};
    this->ts_tt   = {};
    this->ts_tcg  = {};
    this->ts_tcb  = {};
    this->ts_tdb  = {};
    this->utc_tai = get_utc_tai(ts);
    this->dut1    = get_dut1(ts);
    this->jd      = 0.0;
    this->t       = 0.0;
    this->dlt_t   = 0.0;
  } catch (...) {
    throw;
  }
}

/*
 * @brief   JST (日本標準時) 計算
 *
 * @param   <none>
 * @return  JST (timespec)
 */
struct timespec Time::calc_jst() {
  try {
    return utc2jst(ts);
  } catch (...) {
    throw;
  }
}

/*
 * @brief   JD (ユリウス日) 計算
 *
 * @param   <none>
 * @return  JD (double)
 */
double Time::calc_jd() {
  try {
    jd = gc2jd(ts);
    return jd;
  } catch (...) {
    throw;
  }
}

/*
 * @brief   T (ユリウス世紀数) 計算
 *
 * @param   <none>
 * @return  T (double)
 */
double Time::calc_t() {
  try {
    if (jd == 0.0) jd = gc2jd(ts);
    t = jd2t(jd);
    return t;
  } catch (...) {
    throw;
  }
}

/*
 * @brief   UTC - TAI (協定世界時と国際原子時の差 = うるう秒の総和) 計算（返却）
 *
 * @param   <none>
 * @return  UTC - TAI (int)
 */
int Time::calc_utc_tai() { return utc_tai; }

/*
 * @brief   DUT1 (UT1(世界時1) と UTC(協定世界時)の差) 計算（返却）
 *
 * @param   <none>
 * @return  DUT1 (double)
 */
double Time::calc_dut1() { return dut1; }

/*
 * @brief   ΔT (TT(地球時) と UT1(世界時1)の差) 計算
 *
 * @param   <none>
 * @return  ΔT (double)
 */
double Time::calc_dlt_t() {
  struct tm t;
  int    year;  // 西暦年（対象年）
  double y;     // 西暦年（計算用）

  try {
    if (dlt_t != 0.0) return dlt_t;
    if (utc_tai != 0) return kTtTai - utc_tai - dut1;
    localtime_r(&ts.tv_sec, &t);
    year = t.tm_year + 1900;
    y = year + (t.tm_mon + 1 - 0.5) / 12;

    if        (                 year <  -500) {
      dlt_t = calc_dlt_t_bf_m500(y);
    } else if ( -500 <= year && year <   500) {
      dlt_t = calc_dlt_t_bf_0500(y);
    } else if (  500 <= year && year <  1600) {
      dlt_t = calc_dlt_t_bf_1600(y);
    } else if ( 1600 <= year && year <  1700) {
      dlt_t = calc_dlt_t_bf_1700(y);
    } else if ( 1700 <= year && year <  1800) {
      dlt_t = calc_dlt_t_bf_1800(y);
    } else if ( 1800 <= year && year <  1860) {
      dlt_t = calc_dlt_t_bf_1860(y);
    } else if ( 1860 <= year && year <  1900) {
      dlt_t = calc_dlt_t_bf_1900(y);
    } else if ( 1900 <= year && year <  1920) {
      dlt_t = calc_dlt_t_bf_1920(y);
    } else if ( 1920 <= year && year <  1941) {
      dlt_t = calc_dlt_t_bf_1941(y);
    } else if ( 1941 <= year && year <  1961) {
      dlt_t = calc_dlt_t_bf_1961(y);
    } else if ( 1961 <= year && year <  1986) {
      dlt_t = calc_dlt_t_bf_1986(y);
    } else if ( 1986 <= year && year <  2005) {
      dlt_t = calc_dlt_t_bf_2005(y);
    } else if ( 2005 <= year && year <  2050) {
      dlt_t = calc_dlt_t_bf_2050(y);
    } else if ( 2050 <= year && year <= 2150) {
      dlt_t = calc_dlt_t_to_2150(y);
    } else if ( 2150 <  year                ) {
      dlt_t = calc_dlt_t_af_2150(y);
    }
  } catch (...) {
    throw;
  }

  return dlt_t;
}

/*
 * @brief   TAI (国際原子時) 計算
 *
 * @param   <none>
 * @return  TAI (timespec)
 */
struct timespec Time::calc_tai() {
  try {
    ts_tai = utc2tai(ts);
    return ts_tai;
  } catch (...) {
    throw;
  }
}

/*
 * @brief   UT1 (世界時1) 計算
 *
 * @param   <none>
 * @return  UT1 (timespec)
 */
struct timespec Time::calc_ut1() {
  try {
    ts_ut1 = utc2ut1(ts);
    return ts_ut1;
  } catch (...) {
    throw;
  }
}

/*
 * @brief   TT (地球時) 計算
 *
 * @param   <none>
 * @return  TT (timespec)
 */
struct timespec Time::calc_tt() {
  try {
    if (ts_tai.tv_sec == 0) ts_tai = utc2tai(ts);
    ts_tt = tai2tt(ts_tai);
    return ts_tt;
  } catch (...) {
    throw;
  }
}

/*
 * @brief   TCG (地球重心座標時) 計算
 *
 * @param   <none>
 * @return  TCG (timespec)
 */
struct timespec Time::calc_tcg() {
  try {
    if (jd == 0.0) jd = gc2jd(ts);
    if (ts_tai.tv_sec == 0) ts_tai = utc2tai(ts);
    if (ts_tt.tv_sec  == 0) ts_tt  = tai2tt(ts_tai);
    ts_tcg = tt2tcg(ts_tt);
    return ts_tcg;
  } catch (...) {
    throw;
  }
}

/*
 * @brief   TCB (太陽系重心座標時) 計算
 *
 * @param   <none>
 * @return  TCB (timespec)
 */
struct timespec Time::calc_tcb() {
  try {
    if (jd == 0.0) jd = gc2jd(ts);
    if (ts_tai.tv_sec == 0) ts_tai = utc2tai(ts);
    if (ts_tt.tv_sec  == 0) ts_tt  = tai2tt(ts_tai);
    ts_tcb = tt2tcb(ts_tt);
    return ts_tcb;
  } catch (...) {
    throw;
  }
}

/*
 * @brief   TDB (太陽系力学時) 計算
 *
 * @param   <none>
 * @return  TDB (timespec)
 */
struct timespec Time::calc_tdb() {
  try {
    if (jd == 0.0) jd = gc2jd(ts);
    if (ts_tai.tv_sec == 0) ts_tai = utc2tai(ts);
    if (ts_tt.tv_sec  == 0) ts_tt  = tai2tt(ts_tai);
    if (ts_tcb.tv_sec == 0) ts_tcb = tt2tcb(ts_tt);
    ts_tdb = tcb2tdb(ts_tcb);
    return ts_tdb;
  } catch (...) {
    throw;
  }
}

// -------------------------------------
// 以下、 private functions
// -------------------------------------

/*
 * @brief      UTC (協定世界時) -> JST (日本標準時)
 *
 * @param[in]  UTC (timespec)
 * @return     JST (timespec)
 */
struct timespec Time::utc2jst(struct timespec ts) {
  struct timespec ts_jst;

  try {
    ts_jst.tv_sec  = ts.tv_sec + kJstOffset * kSecHour;
    ts_jst.tv_nsec = ts.tv_nsec;
  } catch (...) {
    throw;
  }

  return ts_jst;
}

/*
 * @brief      GC (グレゴリオ暦) -> JD (ユリウス日)
 *
 * @param[in]  GC (timespec)
 * @return     JD (double)
 */
double Time::gc2jd(struct timespec ts) {
  struct tm t;
  unsigned int year;
  unsigned int month;
  unsigned int day;
  unsigned int hour;
  unsigned int min;
  unsigned int sec;
  double jd;

  try {
    localtime_r(&ts.tv_sec, &t);
    year  = t.tm_year + 1900;
    month = t.tm_mon + 1;
    day   = t.tm_mday;
    hour  = t.tm_hour;
    min   = t.tm_min;
    sec   = t.tm_sec;
    // 1月,2月は前年の13月,14月とする
    if (month < 3) {
      --year;
      month += 12;
    }
    // 日付(整数)部分
    jd = static_cast<int>(365.25 * year)
       + static_cast<int>(year / 400.0)
       - static_cast<int>(year / 100.0)
       + static_cast<int>(30.59 * (month - 2))
       + day
       + 1721088.5;
    // 時間(小数)部分
    jd += (sec / 3600.0 + min / 60.0 + hour) / 24.0;
    // 時間(ナノ秒)部分
    jd += ts.tv_nsec / 1000000000.0 / 3600.0 / 24.0;
  } catch (...) {
    throw;
  }

  return jd;
}

/*
 * @brief      JD (ユリウス日) -> T (ユリウス世紀数)
 *
 * @param[in]  JD (double)
 * @return     T (double
 */
double Time::jd2t(double jd_a) {
  double t;

  try {
    t = (jd_a - kJ2000) / (kJy * 100);
  } catch (...) {
    throw;
  }

  return t;
}

/*
 * @brief       UTC - TAI (協定世界時と国際原子時の差 = うるう秒の総和) 取得
 *
 * @param[in]   UTC (timespec)
 * @return      UTC - TAI (int)
 */
int Time::get_utc_tai(struct timespec ts) {
  struct tm t;
  std::stringstream ss;      // 対象年月日算出用
  std::string dt_t;          // 対象年月日
  std::string buf;           // 1行分バッファ
  int i;                     // ループインデックス
  utc_tai = 0;               // 初期化

  try {
    // 対象年月日
    localtime_r(&ts.tv_sec, &t);
    ss << std::setw(4) << std::setfill('0') << std::right
       << t.tm_year + 1900
       << std::setw(2) << std::setfill('0') << std::right
       << t.tm_mon + 1
       << std::setw(2) << std::setfill('0') << std::right
       << t.tm_mday;
    dt_t = ss.str();

    // うるう秒取得
    for (i = l_ls.size() - 1; i >= 0; --i) {
      if (l_ls[i][0] <= dt_t) {
        utc_tai = stoi(l_ls[i][1]);
        break;
      }
    }

    return utc_tai;
  } catch (...) {
    throw;
  }
}

/*
 * @brief       DUT1 (UT1(世界時1) と UTC(協定世界時)の差) 取得
 *
 * @param[ref]  DUT1 一覧
 * @param[in]   UTC (timespec)
 * @return      DUT1 (double)
 */
double Time::get_dut1(struct timespec ts) {
  struct tm t;
  std::stringstream ss;    // 対象年月日算出用
  std::string dt_t;        // 対象年月日
  std::string buf;         // 1行分バッファ
  int i;                   // ループインデックス
  dut1 = 0.0;              // 初期化

  try {
    // 対象年月日
    localtime_r(&ts.tv_sec, &t);
    ss << std::setw(4) << std::setfill('0') << std::right
       << t.tm_year + 1900
       << std::setw(2) << std::setfill('0') << std::right
       << t.tm_mon + 1
       << std::setw(2) << std::setfill('0') << std::right
       << t.tm_mday;
    dt_t = ss.str();

    // DUT1 取得
    for (i = l_dut.size() - 1; i >= 0; --i) {
      if (l_dut[i][0] <= dt_t) {
        dut1 = stod(l_dut[i][1]);
        break;
      }
    }

    return dut1;
  } catch (...) {
    throw;
  }
}

/*
 * @brief     UTC (協定世界時) -> TAI (国際原子時)
 *
 * @param[in]  UTC (timespec)
 * @return     TAI (timespec)
 */
struct timespec Time::utc2tai(struct timespec ts) {
  try {
    ts_tai.tv_sec  = ts.tv_sec - utc_tai;
    ts_tai.tv_nsec = ts.tv_nsec;
  } catch (...) {
    throw;
  }

  return ts_tai;
}

/*
 * @brief      UTC (協定世界時) -> UT1 (世界時1)
 *
 * @param[in]  UTC (timespec)
 * @return     UT1 (timespec)
 */
struct timespec Time::utc2ut1(struct timespec ts) {
  try {
    ts_ut1.tv_sec  = ts.tv_sec;
    ts_ut1.tv_nsec = ts.tv_nsec + dut1 * 1.0e9;
    if (ts_ut1.tv_nsec > 1.0e9) {
      ++ts_ut1.tv_sec;
      ts_ut1.tv_nsec -= 1.0e9;
    } else if (ts_ut1.tv_nsec < 0) {
      --ts_ut1.tv_sec;
      ts_ut1.tv_nsec += 1.0e9;
    }
  } catch (...) {
    throw;
  }

  return ts_ut1;
}

/*
 * @brief      TAI (国際原子時) -> TT (地球時)
 *
 * @param[in]  TAI (timespec)
 * @return     TT (timespec)
 */
struct timespec Time::tai2tt(struct timespec ts) {
  int f_tt_tai;

  try {
    f_tt_tai = floor(kTtTai);
    ts_tt.tv_sec  = ts.tv_sec + f_tt_tai;
    ts_tt.tv_nsec = ts.tv_nsec + (kTtTai - f_tt_tai) * 1.0e9;
    if (ts_tt.tv_nsec > 1.0e9) {
      ++ts_tt.tv_sec;
      ts_tt.tv_nsec -= 1.0e9;
    } else if (ts_tt.tv_nsec < 0) {
      --ts_tt.tv_sec;
      ts_tt.tv_nsec += 1.0e9;
    }
  } catch (...) {
    throw;
  }

  return ts_tt;
}

/*
 * @brief      TT (地球時) -> TCG (地球重心座標時)
 *
 * @param[in]  TT (timespec)
 * @return     TCG (timespec)
 */
struct timespec Time::tt2tcg(struct timespec ts) {
  double v;
  int    f_v;

  try {
    v = kLG * (jd - kT0) * kSecDay;
    f_v = floor(v);
    ts_tcg.tv_sec  = ts.tv_sec + f_v;
    ts_tcg.tv_nsec = ts.tv_nsec + (v - f_v) * 1.0e9;
    if (ts_tcg.tv_nsec > 1.0e9) {
      ++ts_tcg.tv_sec;
      ts_tcg.tv_nsec -= 1.0e9;
    } else if (ts_tcg.tv_nsec < 0) {
      --ts_tcg.tv_sec;
      ts_tcg.tv_nsec += 1.0e9;
    }
  } catch (...) {
    throw;
  }

  return ts_tcg;
}

/*
 * @brief      TT (地球時) -> TCB (太陽系重心座標時)
 *
 * @param[in]  TT (timespec)
 * @return     TCB (timespec)
 */
struct timespec Time::tt2tcb(struct timespec ts) {
  double v;
  int    f_v;

  try {
    v = kLB * (jd - kT0) * kSecDay;
    f_v = floor(v);
    ts_tcb.tv_sec  = ts.tv_sec + f_v;
    ts_tcb.tv_nsec = ts.tv_nsec + (v - f_v) * 1.0e9;
    if (ts_tcb.tv_nsec > 1.0e9) {
      ++ts_tcg.tv_sec;
      ts_tcb.tv_nsec -= 1.0e9;
    } else if (ts_tcb.tv_nsec < 0) {
      --ts_tcb.tv_sec;
      ts_tcb.tv_nsec += 1.0e9;
    }
  } catch (...) {
    throw;
  }

  return ts_tcb;
}

/*
 * @brief      TCB (太陽系重心座標時) -> TDB (太陽系力学時)
 *
 * @param[in]  TCB (timespec)
 * @return     TDB (timespec)
 */
struct timespec Time::tcb2tdb(struct timespec ts) {
  double v;
  int    f_v;

  try {
    v = kLB * (jd - kT0) * kSecDay + kTdb0;
    f_v = floor(v);
    ts_tdb.tv_sec  = ts.tv_sec - f_v;
    ts_tdb.tv_nsec = ts.tv_nsec - (v - f_v) * 1.0e9;
    if (ts_tdb.tv_nsec > 1.0e9) {
      ++ts_tcg.tv_sec;
      ts_tdb.tv_nsec -= 1.0e9;
    } else if (ts_tdb.tv_nsec < 0) {
      --ts_tdb.tv_sec;
      ts_tdb.tv_nsec += 1.0e9;
    }
  } catch (...) {
    throw;
  }

  return ts_tdb;
}

}  // namespace apparent_sun_moon

