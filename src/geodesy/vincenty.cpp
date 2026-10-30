// Vincenty (1975) 타원체 측지선 정문제·역문제.
//
//   T. Vincenty, Direct and inverse solutions of geodesics on the ellipsoid
//   with application of nested equations, Survey Review 23(176), 88-93 (1975).
//
// 보조 구 위의 경도 차 λ(역문제) 또는 호의 길이 σ(정문제)를 고정점 반복으로 구한다.

#include "marine/geodesy.hpp"

#include "angles.hpp"

// 곱셈·덧셈을 FMA 하나로 합치지 않는다. 합치면 같은 점의 거리가 0이 아니게 되는 등
// 상쇄를 노린 식이 어긋난다. SwiftPM은 컴파일 플래그를 줄 수 없어 소스에 둔다.
#if defined(__clang__)
#pragma clang fp contract(off)
#endif

#include <cmath>
#include <numbers>

namespace marine::geodesy {

using detail::to_degrees;
using detail::to_radians;

namespace {

constexpr double kTolerance = 1e-12; // 라디안. 약 0.006mm
constexpr int kMaxIterations = 200;

/// 환산 위도 U: tan U = (1 - f) tan φ. 극에서도 무너지지 않게 sin·cos로 직접 만든다.
void reduced_latitude(double latitude, double f, double& sin_u, double& cos_u) noexcept {
    const double phi = to_radians(latitude);
    const double u = std::atan2((1 - f) * std::sin(phi), std::cos(phi));
    sin_u = std::sin(u);
    cos_u = std::cos(u);
}

/// 식 (3), (4): 거리 급수 계수 A, B.
void series_coefficients(double cos2_alpha, double a, double b, double& A, double& B) noexcept {
    const double u2 = cos2_alpha * (a * a - b * b) / (b * b);
    A = 1 + u2 / 16384 * (4096 + u2 * (-768 + u2 * (320 - 175 * u2)));
    B = u2 / 1024 * (256 + u2 * (-128 + u2 * (74 - 47 * u2)));
}

/// 식 (6): Δσ.
double delta_sigma(double B, double sin_sigma, double cos_sigma, double cos_2sigma_m) noexcept {
    const double c2 = cos_2sigma_m * cos_2sigma_m;
    return B * sin_sigma *
           (cos_2sigma_m + B / 4 *
                               (cos_sigma * (-1 + 2 * c2) -
                                B / 6 * cos_2sigma_m * (-3 + 4 * sin_sigma * sin_sigma) * (-3 + 4 * c2)));
}

/// 식 (10): C.
double lambda_coefficient(double f, double cos2_alpha) noexcept {
    return f / 16 * cos2_alpha * (4 + f * (4 - 3 * cos2_alpha));
}

} // namespace

std::expected<InverseSolution, GeodesyError>
vincenty_inverse(Coordinate from, Coordinate to, const Ellipsoid& ellipsoid) noexcept {
    const double a = ellipsoid.semi_major_axis;
    const double f = ellipsoid.flattening;
    const double b = a * (1 - f);

    double sin_u1, cos_u1, sin_u2, cos_u2;
    reduced_latitude(from.latitude, f, sin_u1, cos_u1);
    reduced_latitude(to.latitude, f, sin_u2, cos_u2);

    const double L = to_radians(detail::normalize_longitude(to.longitude - from.longitude));

    double lambda = L;
    double sin_lambda = 0, cos_lambda = 0;
    double sin_sigma = 0, cos_sigma = 0, sigma = 0;
    double cos2_alpha = 0, cos_2sigma_m = 0;

    int iteration = 0;
    for (;; ++iteration) {
        if (iteration == kMaxIterations) return std::unexpected(GeodesyError::not_converged);

        sin_lambda = std::sin(lambda);
        cos_lambda = std::cos(lambda);
        const double t1 = cos_u2 * sin_lambda;
        const double t2 = cos_u1 * sin_u2 - sin_u1 * cos_u2 * cos_lambda;
        sin_sigma = std::hypot(t1, t2);
        if (sin_sigma == 0) return InverseSolution{0, 0, 0}; // 같은 점

        cos_sigma = sin_u1 * sin_u2 + cos_u1 * cos_u2 * cos_lambda;
        sigma = std::atan2(sin_sigma, cos_sigma);
        const double sin_alpha = cos_u1 * cos_u2 * sin_lambda / sin_sigma;
        cos2_alpha = 1 - sin_alpha * sin_alpha;
        // 적도를 따라가는 선은 cos²α = 0이고 cos 2σm은 쓰이지 않는다.
        cos_2sigma_m = cos2_alpha != 0 ? cos_sigma - 2 * sin_u1 * sin_u2 / cos2_alpha : 0;

        const double C = lambda_coefficient(f, cos2_alpha);
        const double previous = lambda;
        lambda = L + (1 - C) * f * sin_alpha *
                         (sigma + C * sin_sigma *
                                      (cos_2sigma_m + C * cos_sigma * (-1 + 2 * cos_2sigma_m * cos_2sigma_m)));

        // 대척점 근처에서는 λ가 π를 넘어 흔들리며 수렴하지 않는다.
        if (std::fabs(lambda) > std::numbers::pi) return std::unexpected(GeodesyError::not_converged);
        if (std::fabs(lambda - previous) < kTolerance) break;
    }

    double A, B;
    series_coefficients(cos2_alpha, a, b, A, B);
    const double distance = b * A * (sigma - delta_sigma(B, sin_sigma, cos_sigma, cos_2sigma_m));

    const double alpha1 = std::atan2(cos_u2 * sin_lambda, cos_u1 * sin_u2 - sin_u1 * cos_u2 * cos_lambda);
    const double alpha2 = std::atan2(cos_u1 * sin_lambda, -sin_u1 * cos_u2 + cos_u1 * sin_u2 * cos_lambda);

    return InverseSolution{
        distance,
        detail::normalize_bearing(to_degrees(alpha1)),
        detail::normalize_bearing(to_degrees(alpha2)),
    };
}

std::expected<DirectSolution, GeodesyError>
vincenty_direct(Coordinate from, double bearing, double distance, const Ellipsoid& ellipsoid) noexcept {
    const double a = ellipsoid.semi_major_axis;
    const double f = ellipsoid.flattening;
    const double b = a * (1 - f);

    const double alpha1 = to_radians(bearing);
    const double sin_alpha1 = std::sin(alpha1);
    const double cos_alpha1 = std::cos(alpha1);

    double sin_u1, cos_u1;
    reduced_latitude(from.latitude, f, sin_u1, cos_u1);

    const double sigma1 = std::atan2(sin_u1, cos_u1 * cos_alpha1); // 식 (1)
    const double sin_alpha = cos_u1 * sin_alpha1;                  // 식 (2)
    const double cos2_alpha = 1 - sin_alpha * sin_alpha;

    double A, B;
    series_coefficients(cos2_alpha, a, b, A, B);

    double sigma = distance / (b * A);
    double sin_sigma = 0, cos_sigma = 0, cos_2sigma_m = 0;

    int iteration = 0;
    for (;; ++iteration) {
        if (iteration == kMaxIterations) return std::unexpected(GeodesyError::not_converged);

        cos_2sigma_m = std::cos(2 * sigma1 + sigma); // 식 (5)
        sin_sigma = std::sin(sigma);
        cos_sigma = std::cos(sigma);
        const double previous = sigma;
        sigma = distance / (b * A) + delta_sigma(B, sin_sigma, cos_sigma, cos_2sigma_m); // 식 (7)
        if (std::fabs(sigma - previous) < kTolerance) break;
    }
    sin_sigma = std::sin(sigma);
    cos_sigma = std::cos(sigma);
    cos_2sigma_m = std::cos(2 * sigma1 + sigma);

    const double x = sin_u1 * sin_sigma - cos_u1 * cos_sigma * cos_alpha1;
    const double phi2 = std::atan2(sin_u1 * cos_sigma + cos_u1 * sin_sigma * cos_alpha1,
                                   (1 - f) * std::hypot(sin_alpha, x)); // 식 (8)
    const double lambda = std::atan2(sin_sigma * sin_alpha1,
                                     cos_u1 * cos_sigma - sin_u1 * sin_sigma * cos_alpha1); // 식 (9)
    const double C = lambda_coefficient(f, cos2_alpha);
    const double L = lambda - (1 - C) * f * sin_alpha *
                                  (sigma + C * sin_sigma *
                                               (cos_2sigma_m + C * cos_sigma * (-1 + 2 * cos_2sigma_m * cos_2sigma_m))); // 식 (11)
    const double alpha2 = std::atan2(sin_alpha, -x); // 식 (12)

    return DirectSolution{
        {to_degrees(phi2), detail::normalize_longitude(from.longitude + to_degrees(L))},
        detail::normalize_bearing(to_degrees(alpha2)),
    };
}

} // namespace marine::geodesy
