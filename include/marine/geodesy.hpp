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
    /// 입력이 잘못됐다. 위도가 [-90, 90] 밖이거나, 위도·경도·방위·거리가 NaN·무한대이거나,
    /// 타원체가 장반경 > 0, 편평률 [0, 1)을 벗어났다.
    invalid_argument,
};

// 구면 (하버사인) ---------------------------------------------------------

// 실패하지 않는 함수는 잘못된 입력(invalid_argument와 같은 조건)에 NaN을 돌려준다.

/// 구 위 두 점 사이의 대권 거리(m). 하버사인 공식. 잘못된 좌표면 NaN.
double haversine_distance(Coordinate from, Coordinate to) noexcept;

/// from에서 to로 향하는 대권의 처음 방위(도, 진북 기준 시계 방향 [0, 360)). 잘못된 좌표면 NaN.
double initial_bearing(Coordinate from, Coordinate to) noexcept;

// 타원체 (Vincenty 1975) --------------------------------------------------

/// 두 점 사이의 측지선 거리와 방위. 대척점 근처에서 수렴하지 않으면 not_converged,
/// 잘못된 입력이면 invalid_argument.
std::expected<InverseSolution, GeodesyError>
vincenty_inverse(Coordinate from, Coordinate to, const Ellipsoid& ellipsoid = kWGS84) noexcept;

/// 출발점에서 bearing(도) 방향으로 distance(m)만큼 간 도착점. 잘못된 입력이면 invalid_argument.
std::expected<DirectSolution, GeodesyError>
vincenty_direct(Coordinate from, double bearing, double distance,
                const Ellipsoid& ellipsoid = kWGS84) noexcept;

} // namespace marine::geodesy
