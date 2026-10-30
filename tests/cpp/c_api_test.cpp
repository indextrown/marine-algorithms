#include "marine_c.h"

#include "marine/geodesy.hpp"
#include "marine/units.hpp"

#include <catch2/catch_test_macros.hpp>

// C 다리층은 코어 결과를 그대로 넘겨야 한다. 정확도는 코어 테스트에서 본다.

namespace geo = marine::geodesy;

namespace {

const ma_coordinate kBusan{35.1028, 129.0403};
const ma_coordinate kFukuoka{33.5902, 130.4017};
const geo::Coordinate kBusanCpp{kBusan.latitude, kBusan.longitude};
const geo::Coordinate kFukuokaCpp{kFukuoka.latitude, kFukuoka.longitude};

} // namespace

TEST_CASE("C API: 단위 변환") {
    CHECK(ma_meters_to_nautical_miles(208'000) == marine::units::meters_to_nautical_miles(208'000));
    CHECK(ma_nautical_miles_to_meters(3.5) == marine::units::nautical_miles_to_meters(3.5));
    CHECK(ma_knots_to_meters_per_second(12) == marine::units::knots_to_meters_per_second(12));
    CHECK(ma_meters_per_second_to_knots(6.2) == marine::units::meters_per_second_to_knots(6.2));
}

TEST_CASE("C API: 하버사인") {
    CHECK(ma_haversine_distance(kBusan, kFukuoka) == geo::haversine_distance(kBusanCpp, kFukuokaCpp));
    CHECK(ma_initial_bearing(kBusan, kFukuoka) == geo::initial_bearing(kBusanCpp, kFukuokaCpp));
}

TEST_CASE("C API: WGS84") {
    const ma_ellipsoid e = ma_wgs84();
    CHECK(e.semi_major_axis == geo::kWGS84.semi_major_axis);
    CHECK(e.flattening == geo::kWGS84.flattening);
}

TEST_CASE("C API: Vincenty") {
    SECTION("역문제") {
        ma_inverse out{};
        REQUIRE(ma_vincenty_inverse(kBusan, kFukuoka, ma_wgs84(), &out) == MA_OK);
        const auto expected = geo::vincenty_inverse(kBusanCpp, kFukuokaCpp);
        REQUIRE(expected);
        CHECK(out.distance == expected->distance);
        CHECK(out.initial_bearing == expected->initial_bearing);
        CHECK(out.final_bearing == expected->final_bearing);
    }
    SECTION("정문제") {
        ma_direct out{};
        REQUIRE(ma_vincenty_direct(kBusan, 135, 500'000, ma_wgs84(), &out) == MA_OK);
        const auto expected = geo::vincenty_direct(kBusanCpp, 135, 500'000);
        REQUIRE(expected);
        CHECK(out.destination.latitude == expected->destination.latitude);
        CHECK(out.destination.longitude == expected->destination.longitude);
        CHECK(out.final_bearing == expected->final_bearing);
    }
    SECTION("수렴하지 않으면 MA_NOT_CONVERGED, out은 그대로") {
        ma_inverse out{-1, -1, -1};
        CHECK(ma_vincenty_inverse({0, 0}, {0.5, 179.7}, ma_wgs84(), &out) == MA_NOT_CONVERGED);
        CHECK(out.distance == -1);
    }
    SECTION("타원체를 넘긴다: 편평률 0이면 구") {
        const ma_ellipsoid sphere{geo::kEarthRadiusMeters, 0};
        ma_inverse out{};
        REQUIRE(ma_vincenty_inverse(kBusan, kFukuoka, sphere, &out) == MA_OK);
        const auto expected = geo::vincenty_inverse(kBusanCpp, kFukuokaCpp, {geo::kEarthRadiusMeters, 0});
        REQUIRE(expected);
        CHECK(out.distance == expected->distance);
    }
    SECTION("out이 NULL이면 MA_INVALID_ARGUMENT") {
        CHECK(ma_vincenty_inverse(kBusan, kFukuoka, ma_wgs84(), nullptr) == MA_INVALID_ARGUMENT);
        CHECK(ma_vincenty_direct(kBusan, 0, 1000, ma_wgs84(), nullptr) == MA_INVALID_ARGUMENT);
    }
}
