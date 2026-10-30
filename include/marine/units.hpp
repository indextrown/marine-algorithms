#pragma once

namespace marine::units {

/// 1해리(nm) = 1852m. 국제 해리, 정의값.
inline constexpr double kMetersPerNauticalMile = 1852.0;

/// 1노트(kn) = 1시간에 1해리 = 1852/3600 m/s.
inline constexpr double kMetersPerSecondPerKnot = kMetersPerNauticalMile / 3600.0;

/// m → 해리
constexpr double meters_to_nautical_miles(double meters) noexcept { return meters / kMetersPerNauticalMile; }

/// 해리 → m
constexpr double nautical_miles_to_meters(double nautical_miles) noexcept {
    return nautical_miles * kMetersPerNauticalMile;
}

/// 노트 → m/s
constexpr double knots_to_meters_per_second(double knots) noexcept { return knots * kMetersPerSecondPerKnot; }

/// m/s → 노트
constexpr double meters_per_second_to_knots(double meters_per_second) noexcept {
    return meters_per_second / kMetersPerSecondPerKnot;
}

} // namespace marine::units
