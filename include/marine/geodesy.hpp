#pragma once

#include <expected>

namespace marine::geodesy {

/// 평균 지구 반지름(m). IUGG 평균 반지름 R1.
inline constexpr double kEarthRadiusMeters = 6371008.8;

/// 위도·경도(도 단위). 위도 [-90, 90], 경도 [-180, 180].
struct Coordinate {
    double latitude;
    double longitude;
};

/// 회전 타원체. 장반경 a(m)와 편평률 f.
struct Ellipsoid {
    double semi_major_axis;
    double flattening;
};

/// GPS가 쓰는 WGS84 타원체.
inline constexpr Ellipsoid kWGS84{6378137.0, 1 / 298.257223563};

/// 역문제의 답. 방위는 진북 기준 시계 방향 [0, 360).
struct InverseSolution {
    double distance;        ///< 측지선 길이(m)
    double initial_bearing; ///< 출발점에서의 방위
    double final_bearing;   ///< 도착점에서의 방위(진행 방향)
};

/// 정문제의 답.
struct DirectSolution {
    Coordinate destination;
    double final_bearing; ///< 도착점에서의 방위(진행 방향) [0, 360)
};

enum class GeodesyError {
    /// 반복 계산이 수렴하지 않았다. Vincenty 역문제에서 두 점이 거의 대척점일 때 생긴다.
    not_converged,
};

// 구면 (하버사인) ---------------------------------------------------------

/// 구 위 두 점 사이의 대권 거리(m). 하버사인 공식.
double haversine_distance(Coordinate from, Coordinate to) noexcept;

/// from에서 to로 향하는 대권의 처음 방위(도, 진북 기준 시계 방향 [0, 360)).
double initial_bearing(Coordinate from, Coordinate to) noexcept;

// 타원체 (Vincenty 1975) --------------------------------------------------

/// 두 점 사이의 측지선 거리와 방위. 대척점 근처에서 수렴하지 않으면 not_converged.
std::expected<InverseSolution, GeodesyError>
vincenty_inverse(Coordinate from, Coordinate to, const Ellipsoid& ellipsoid = kWGS84) noexcept;

/// 출발점에서 bearing(도) 방향으로 distance(m)만큼 간 도착점.
std::expected<DirectSolution, GeodesyError>
vincenty_direct(Coordinate from, double bearing, double distance,
                const Ellipsoid& ellipsoid = kWGS84) noexcept;

} // namespace marine::geodesy
