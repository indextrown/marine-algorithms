#pragma once

#include <cmath>
#include <numbers>

// geodesy 구현끼리 나눠 쓰는 각도 도우미. 공개 헤더가 아니다.
namespace marine::geodesy::detail {

constexpr double to_radians(double degrees) noexcept { return degrees * std::numbers::pi / 180.0; }
constexpr double to_degrees(double radians) noexcept { return radians * 180.0 / std::numbers::pi; }

/// 방위를 [0, 360)으로.
inline double normalize_bearing(double degrees) noexcept {
    double r = std::fmod(degrees, 360.0);
    if (r < 0) r += 360.0;
    // -1e-20 + 360 처럼 반올림으로 360이 되는 경우.
    return r >= 360.0 ? 0.0 : r + 0.0;
}

/// 경도를 [-180, 180]으로.
inline double normalize_longitude(double degrees) noexcept {
    return std::remainder(degrees, 360.0);
}

} // namespace marine::geodesy::detail
