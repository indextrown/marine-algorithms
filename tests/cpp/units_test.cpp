#include "marine/units.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace marine::units;
using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

TEST_CASE("해리") {
    CHECK(nautical_miles_to_meters(1) == 1852.0);
    CHECK(meters_to_nautical_miles(1852) == 1.0);
    CHECK_THAT(meters_to_nautical_miles(208'000), WithinRel(112.311015, 1e-8));
}

TEST_CASE("노트") {
    CHECK_THAT(knots_to_meters_per_second(1), WithinRel(0.514444444444, 1e-12));
    CHECK_THAT(knots_to_meters_per_second(12), WithinRel(6.173333333333, 1e-12));
    // 1 m/s ≈ 1.943844 kn
    CHECK_THAT(meters_per_second_to_knots(1), WithinRel(1.943844492441, 1e-12));
}

TEST_CASE("왕복하면 원래 값") {
    for (double x : {0.0, 0.1, 1.0, 12.5, 1'000.0, 20'000'000.0}) {
        CHECK_THAT(nautical_miles_to_meters(meters_to_nautical_miles(x)), WithinRel(x, 1e-15) || WithinAbs(x, 0));
        CHECK_THAT(meters_per_second_to_knots(knots_to_meters_per_second(x)), WithinRel(x, 1e-15) || WithinAbs(x, 0));
    }
}

TEST_CASE("음수는 부호를 유지한다") {
    CHECK(meters_to_nautical_miles(-1852) == -1.0);
    CHECK(knots_to_meters_per_second(-1) == -kMetersPerSecondPerKnot);
}

// 컴파일 시점에도 계산된다.
static_assert(nautical_miles_to_meters(2) == 3704.0);
static_assert(meters_to_nautical_miles(3704) == 2.0);
