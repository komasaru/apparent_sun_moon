#include "file.hpp"

namespace apparent_sun_moon {
// 定数
static constexpr char kFLeapSec[13] = "LEAP_SEC.txt";
static constexpr char kFDut1[9]     = "DUT1.txt";
static constexpr char kFNutLs[11]   = "NUT_LS.txt";
static constexpr char kFNutPl[11]   = "NUT_PL.txt";

/*
 * @brief      UTC - TAI (協定世界時と国際原子時の差 = うるう秒の総和) 一覧取得
 *
 * @param[ref] UTC - TAI 一覧(vector<vector<string>>)
 * @return     true|false
 */
bool File::get_leap_sec_list(std::vector<std::vector<std::string>>& data) {
  std::string f(kFLeapSec);  // ファイル名
  std::string buf;           // 1行分バッファ

  try {
    // ファイル OPEN
    std::ifstream ifs(f);
    if (!ifs) return 0;  // 読み込み失敗

    // ファイル READ
    while (getline(ifs, buf)) {
      std::vector<std::string> rec;  // 1行分ベクタ
      std::istringstream iss(buf);   // 文字列ストリーム
      // 1行分文字列を1行分ベクタに追加
      std::string s;
      while (iss >> s) rec.push_back(s);
      // 1行分ベクタを data ベクタに追加
      if (rec.size() != 0) data.push_back(rec);
    }
  } catch (...) {
    return false;
  }

  return true;
}

/*
 * @brief      DUT1 (UT1(世界時1) と UTC(協定世界時)の差) 一覧取得
 *
 * @param[ref] DUT1 一覧(vector<vector<string>>)
 * @return     true|false
 */
bool File::get_dut1_list(std::vector<std::vector<std::string>>& data) {
  std::string f(kFDut1);  // ファイル名
  std::string buf;        // 1行分バッファ

  try {
    // ファイル OPEN
    std::ifstream ifs(f);
    if (!ifs) return 0;  // 読み込み失敗

    // ファイル READ
    while (getline(ifs, buf)) {
      std::vector<std::string> rec;  // 1行分ベクタ
      std::istringstream iss(buf);   // 文字列ストリーム
      // 1行分文字列を1行分ベクタに追加
      std::string s;
      while (iss >> s) rec.push_back(s);
      // 1行分ベクタを data ベクタに追加
      if (rec.size() != 0) data.push_back(rec);
    }
  } catch (...) {
    return false;
  }

  return true;
}

/*
 * @brief      取得: lunisolar parameters
 *             （第6列以後は 10,000 倍にする）
 *
 * @param[ref] lunisolar パラメータ一覧(vector<vector<double>>)
 * @return     true|false
 */
bool File::get_param_ls(std::vector<std::vector<double>>& data) {
  std::string f(kFNutLs);  // ファイル名
  std::string buf;         // 1行分バッファ
  unsigned int c;          // ループインデックス（列処理用

  try {
    // ファイル OPEN
    std::ifstream ifs(f);
    if (!ifs) return 0;  // 読み込み失敗

    // ファイル READ
    while (getline(ifs, buf)) {
      std::vector<double> rec;      // 1行分ベクタ
      std::istringstream iss(buf);  // 文字列ストリーム
      // 1行分文字列を1行分ベクタに追加
      double s;
      c = 0;
      while (iss >> s) {
        if (c > 4) s *= 10000;
        rec.push_back(s);
        ++c;
      }
      // 1行分ベクタを data ベクタに追加
      if (rec.size() != 0) data.push_back(rec);
    }
    return data.size();
  } catch (...) {
    return false;
  }

  return true;
}

/*
 * @brief      取得: planetary parameters
 *             （第15列以後は 10,000 倍にする）
 *
 * @param[ref] planetary パラメータ一覧(vector<vector<double>>)
 * @return     true|false
 */
bool File::get_param_pl(std::vector<std::vector<double>>& data) {
  std::string f(kFNutPl);  // ファイル名
  std::string buf;         // 1行分バッファ
  unsigned int c;          // ループインデックス（列処理用

  try {
    // ファイル OPEN
    std::ifstream ifs(f);
    if (!ifs) return 0;  // 読み込み失敗

    // ファイル READ
    while (getline(ifs, buf)) {
      std::vector<double> rec;      // 1行分ベクタ
      std::istringstream iss(buf);  // 文字列ストリーム
      // 1行分文字列を1行分ベクタに追加
      double s;
      c = 0;
      while (iss >> s) {
        if (c > 13) s *= 10000;
        rec.push_back(s);
        ++c;
      }
      // 1行分ベクタを data ベクタに追加
      if (rec.size() != 0) data.push_back(rec);
    }
    return data.size();
  } catch (...) {
    return false;
  }

  return true;
}

}  // namespace apparent_sun_moon

