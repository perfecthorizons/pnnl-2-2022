#ifndef _LITERALS_H
#define _LITERALS_H

#include <chrono>

constexpr std::chrono::nanoseconds operator""_hz(long double hz) {
  return std::chrono::nanoseconds( static_cast<int>(1000000000/hz) );
}

constexpr std::chrono::nanoseconds operator""_hz(uint64_t hz) {
  return std::chrono::nanoseconds( static_cast<int>(1000000000.0/hz) );
}

#endif
