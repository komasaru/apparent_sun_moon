#ifndef APPARENT_SUN_MOON_FILE_HPP_
#define APPARENT_SUN_MOON_FILE_HPP_

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace apparent_sun_moon {

class File {

public:
  bool get_leap_sec_list(std::vector<std::vector<std::string>>&);  // 取得: うるう秒一覧
  bool get_dut1_list(std::vector<std::vector<std::string>>&);      // 取得: DUT1 一覧
  bool get_param_ls(std::vector<std::vector<double>>&);            // 取得: lunisolar parameters
  bool get_param_pl(std::vector<std::vector<double>>&);            // 取得: planetary parameters
};

}  // namespace apparent_sun_moon

#endif

