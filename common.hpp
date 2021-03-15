#ifndef APPARENT_SUN_MOON_COMMON_HPP_
#define APPARENT_SUN_MOON_COMMON_HPP_

#include <ctime>
#include <iomanip>
#include <string>

namespace apparent_sun_moon {

// -------------------------------------
//   Functions
// -------------------------------------
struct timespec jst2utc(struct timespec);   // 変換: JST -> UTC
std::string gen_time_str(struct timespec);  // 日時文字列生成

}  // namespace apparent_sun_moon

#endif

