#pragma once
#include <chrono>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>

namespace DateTimeUtils {
inline std::optional<std::chrono::system_clock::time_point>
parseDateTime(const std::string &s) {
  std::tm tm{};
  std::istringstream in(s);
  in >> std::get_time(&tm, "%Y-%m-%d_%H:%M");
  if (in.fail()) {
    return std::nullopt;
  }
  tm.tm_isdst = -1;
  std::time_t tt = std::mktime(&tm);
  if (tt == -1) {
    return std::nullopt;
  }
  return std::chrono::system_clock::from_time_t(tt);
}

inline std::string
formatDateTime(const std::chrono::system_clock::time_point &tp) {
  std::time_t tt = std::chrono::system_clock::to_time_t(tp);
  std::tm tm = *std::localtime(&tt);
  std::ostringstream out;
  out << std::put_time(&tm, "%Y-%m-%d_%H:%M");
  return out.str();
}
}