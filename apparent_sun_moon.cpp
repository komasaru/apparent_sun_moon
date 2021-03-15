/***********************************************************
  太陽・月の視位置計算

  * JPLEPH(JPL の DE430 バイナリデータ)読み込み、視位置を計算する
    (自作 RubyGems ライブラリ mk_apos を使用)

    DATE        AUTHOR       VERSION
    2021.01.11  mk-mode.com  1.00 新規作成

  Copyright(C) 2021 mk-mode.com All Rights Reserved.
----------------------------------------------------------
  引数 : JST（日本標準時）
           書式：最大23桁の数字
                 （先頭から、西暦年(4), 月(2), 日(2), 時(2), 分(2), 秒(2),
                             1秒未満(9)（小数点以下9桁（ナノ秒）まで））
                 無指定なら現在(システム日時)と判断。
***********************************************************/
#include "apos.hpp"
#include "common.hpp"
#include "file.hpp"
#include "position.hpp"

#include <cstdlib>   // for EXIT_XXXX
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  static constexpr double kPi = atan(1.0) * 4;  // 円周率
  namespace ns = apparent_sun_moon;
  std::string tm_str;   // time string
  unsigned int s_tm;    // size of time string
  int s_nsec;           // size of nsec string
  int ret;              // return of functions
  struct timespec jst;  // JST
  struct timespec utc;  // UTC
  struct tm t = {};     // for work
  ns::Position pos_s;   // 視位置（太陽）
  ns::Position pos_m;   // 視位置（月）
  std::vector<std::vector<std::string>> l_ls;    // List of Leap Second
  std::vector<std::vector<std::string>> l_dut;   // List of DUT1
  std::vector<std::vector<double>>      dat_ls;  // data of lunisolar parameters
  std::vector<std::vector<double>>      dat_pl;  // data of planetary parameters

  try {
    // 日付取得
    if (argc > 1) {
      // コマンドライン引数より取得
      tm_str = argv[1];
      s_tm = tm_str.size();
      if (s_tm > 23) {
        std::cout << "[ERROR] Over 23-digits!" << std::endl;
        return EXIT_FAILURE;
      }
      s_nsec = s_tm - 14;
      std::istringstream is(tm_str);
      is >> std::get_time(&t, "%Y%m%d%H%M%S");
      jst.tv_sec  = mktime(&t);
      jst.tv_nsec = 0;
      if (s_tm > 14) {
        jst.tv_nsec = std::stod(
            tm_str.substr(14, s_nsec) + std::string(9 - s_nsec, '0'));
      }
    } else {
      // 現在日時の取得
      ret = std::timespec_get(&jst, TIME_UTC);
      if (ret != 1) {
        std::cout << "[ERROR] Could not get now time!" << std::endl;
        return EXIT_FAILURE;
      }
    }

    // うるう秒, DUT1 一覧、
    // lunisolra, planetary パラメータ一覧取得
    ns::File o_f;
    if (!o_f.get_leap_sec_list(l_ls)) throw;
    if (!o_f.get_dut1_list(l_dut))    throw;
    if (!o_f.get_param_ls(dat_ls))    throw;
    if (!o_f.get_param_pl(dat_pl))    throw;

    // JST -> UTC
    utc = ns::jst2utc(jst);

    // 視位置計算
    ns::Apos o_a(utc, l_ls, l_dut, dat_ls, dat_pl);
    pos_s = o_a.sun();
    pos_m = o_a.moon();

    // 結果出力
    std::cout << "            JST: "
              << ns::gen_time_str(jst) << std::endl;
    std::cout << "            UTC: "
              << ns::gen_time_str(utc) << std::endl;
    std::cout << "            TDB: "
              << ns::gen_time_str(o_a.tdb) << std::endl;
    std::cout << "        JD(TDB): "
              << std::fixed << std::setprecision(8)
              << o_a.jd << " day" << std::endl;
    std::cout << "---" << std::endl
              << std::fixed << std::setprecision(10);
    std::cout << "* 視位置: 太陽" << std::endl
              << "  = [赤経: "
              << std::setw(14) << pos_s.alpha
              << " rad, 赤緯: "
              << std::setw(14) << pos_s.delta
              << " rad]" << std::endl
              << "  = [赤経: "
              << std::setw(14) << pos_s.alpha * 180.0 / kPi
              << " deg, 赤緯: "
              << std::setw(14) << pos_s.delta * 180.0 / kPi
              << " deg]" << std::endl
              << "  = [黄経: "
              << std::setw(14) << pos_s.lambda
              << " rad, 黄緯: "
              << std::setw(14) << pos_s.beta
              << " rad]" << std::endl
              << "  = [黄経: "
              << std::setw(14) << pos_s.lambda * 180.0 / kPi
              << " deg, 黄緯: "
              << std::setw(14) << pos_s.beta   * 180.0 / kPi
              << " deg]" << std::endl;
    std::cout << "* 視位置: 月" << std::endl
              << "  = [赤経: "
              << std::setw(14) << pos_m.alpha
              << " rad, 赤緯: "
              << std::setw(14) << pos_m.delta
              << " rad]" << std::endl
              << "  = [赤経: "
              << std::setw(14) << pos_m.alpha * 180.0 / kPi
              << " deg, 赤緯: "
              << std::setw(14) << pos_m.delta * 180.0 / kPi
              << " deg]" << std::endl
              << "  = [黄経: "
              << std::setw(14) << pos_m.lambda
              << " rad, 黄緯: "
              << std::setw(14) << pos_m.beta
              << " rad]" << std::endl
              << "  = [黄経: "
              << std::setw(14) << pos_m.lambda * 180.0 / kPi
              << " deg, 黄緯: "
              << std::setw(14) << pos_m.beta   * 180.0 / kPi
              << " deg]" << std::endl;
    std::cout << "* 距離: 太陽" << std::endl
              << "  = " << pos_s.d_ec << " AU" << std::endl;
    std::cout << "* 距離: 月" << std::endl
              << "  = " << pos_m.d_ec << " AU" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "* 視半径: 太陽" << std::endl
              << "  = " << pos_s.a_radius << " ″" << std::endl;
    std::cout << "* 視半径: 月" << std::endl
              << "  = " << pos_m.a_radius << " ″" << std::endl;
    std::cout << "* （地平）視差: 太陽" << std::endl
              << "  = " << pos_s.parallax << " ″" << std::endl;
    std::cout << "* （地平）視差: 月" << std::endl
              << "  = " << pos_m.parallax << " ″" << std::endl;
  } catch (...) {
      std::cerr << "EXCEPTION!" << std::endl;
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

