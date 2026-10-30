#include "marine/geodesy.hpp"

#include "angles.hpp"

// 곱셈·덧셈을 FMA 하나로 합치지 않는다. 합치면 같은 점의 거리가 0이 아니게 되는 등
// 상쇄를 노린 식이 어긋난다. SwiftPM은 컴파일 플래그를 줄 수 없어 소스에 둔다.
#if defined(__clang__)
#pragma clang fp contract(off)
#endif

#include <cmath>

namespace marine::geodesy {

using detail::to_radians;
using detail::to_degrees;

double haversine_distance(Coordinate from, Coordinate to) noexcept {
    const double phi1 = to_radians(from.latitude);
    const double phi2 = to_radians(to.latitude);
    const double dphi = phi2 - phi1;
    const double dlambda = to_radians(to.longitude - from.longitude);

    const double a = std::sin(dphi / 2) * std::sin(dphi / 2) +
                     std::cos(phi1) * std::cos(phi2) * std::sin(dlambda / 2) * std::sin(dlambda / 2);
    // 반올림 오차로 a가 1을 살짝 넘으면 sqrt(1 - a)가 NaN이 된다.
    const double c = 2 * std::atan2(std::sqrt(a), std::sqrt(std::fmax(0.0, 1 - a)));
    return kEarthRadiusMeters * c;
}

double initial_bearing(Coordinate from, Coordinate to) noexcept {
    const double phi1 = to_radians(from.latitude);
    const double phi2 = to_radians(to.latitude);
    const double dlambda = to_radians(to.longitude - from.longitude);

    const double y = std::sin(dlambda) * std::cos(phi2);
    const double x = std::cos(phi1) * std::sin(phi2) - std::sin(phi1) * std::cos(phi2) * std::cos(dlambda);
    return detail::normalize_bearing(to_degrees(std::atan2(y, x)));
}

} // namespace marine::geodesy
