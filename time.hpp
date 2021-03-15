#ifndef APPARENT_SUN_MOON_TIME_HPP_
#define APPARENT_SUN_MOON_TIME_HPP_

#include "delta_t.hpp"

#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace apparent_sun_moon {

class Time {
  std::vector<std::vector<std::string>> l_ls;   // List of Leap Second
  std::vector<std::vector<std::string>> l_dut;  // List of DUT1
  struct timespec ts;      // timespec of UTC
  struct timespec ts_tai;  // timespec of TAI
  struct timespec ts_ut1;  // timespec of UT1
  struct timespec ts_tt;   // timespec of TT
  struct timespec ts_tcg;  // timespec of TCG
  struct timespec ts_tcb;  // timespec of TCB
  struct timespec ts_tdb;  // timespec of TDB
  double jd;               // JD (ユリウス日)
  double t;                // T (ユリウス世紀数)
  double dut1;             // UTC - TAI (協定世界時と国際原子時の差 = うるう秒の総和)
  double dlt_t;            // ΔT (TT(地球時) と UT1(世界時1)の差)
  int    utc_tai;          // UTC - TAI (協定世界時と国際原子時の差 = うるう秒の総和)

public:
  Time(
      struct timespec,
      std::vector<std::vector<std::string>>&,
      std::vector<std::vector<std::string>>&);  // コンストラクタ
  struct timespec calc_jst();  // 計算: JST  (日本標準時)
  double calc_jd();            // 計算: JD   (ユリウス日)
  double calc_t();             // 計算: T    (ユリウス世紀数)
  int    calc_utc_tai();       // 計算: UTC - TAI (協定世界時と国際原子時の差 = うるう秒の総和)
  double calc_dut1();          // 計算: DUT1 (UT1(世界時1) と UTC(協定世界時)の差)
  double calc_dlt_t();         // 計算: ΔT  (TT(地球時) と UT1(世界時1)の差)
  struct timespec calc_tai();  // 計算: TAI  (国際原子時)
  struct timespec calc_ut1();  // 計算: UT1  (世界時1)
  struct timespec calc_tt();   // 計算: TT   (地球時)
  struct timespec calc_tcg();  // 計算: TCG  (地球重心座標時)
  struct timespec calc_tcb();  // 計算: TCB  (太陽系重心座標時)
  struct timespec calc_tdb();  // 計算: TDB  (太陽系力学時)

private:
  struct timespec utc2jst(struct timespec);  // UTC -> JST
  double gc2jd(struct timespec);             // GC  -> JD
  double jd2t(double);                       // JD  -> T
  int    get_utc_tai(struct timespec);       // UTC -> UTC - TAI
  double get_dut1(struct timespec);          // UTC -> DUT1
  struct timespec utc2tai(struct timespec);  // UTC -> TAI
  struct timespec utc2ut1(struct timespec);  // UTC -> UT1
  struct timespec tai2tt(struct timespec);   // TAI -> TT
  struct timespec tt2tcg(struct timespec);   // TT  -> TCG
  struct timespec tt2tcb(struct timespec);   // TT  -> TCB
  struct timespec tcb2tdb(struct timespec);  // TCB -> TDB
};

}  // namespace apparent_sun_moon

#endif

