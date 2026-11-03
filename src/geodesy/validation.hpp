#pragma once

#include "marine/geodesy.hpp"

#include <cmath>

// geodesy 구현끼리 나눠 쓰는 입력 검사. 공개 헤더가 아니다.
namespace marine::geodesy::detail {

/// 위도는 [-90, 90], 경도는 유한값. NaN은 비교가 모두 거짓이라 걸러진다.
inline bool is_valid(Coordinate c) noexcept {
    return c.latitude >= -90.0 && c.latitude <= 90.0 && std::isfinite(c.longitude);
}

/// 장반경은 양의 유한값, 편평률은 [0, 1).
inline bool is_valid(const Ellipsoid& e) noexcept {
    return std::isfinite(e.semi_major_axis) && e.semi_major_axis > 0 && e.flattening >= 0 &&
           e.flattening < 1;
}

} // namespace marine::geodesy::detail
