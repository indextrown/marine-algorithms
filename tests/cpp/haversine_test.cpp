#include "marine/geodesy.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using marine::geodesy::Coordinate;
using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

TEST_CASE("haversine_distance") {
    const Coordinate busan{35.1028, 129.0403};
    const Coordinate fukuoka{33.5902, 130.4017};

    SECTION("같은 점은 0") {
        CHECK(marine::geodesy::haversine_distance(busan, busan) == 0.0);
    }
    SECTION("부산-후쿠오카 약 208km") {
        CHECK_THAT(marine::geodesy::haversine_distance(busan, fukuoka), WithinAbs(208'000, 2'000));
    }
    SECTION("구면 근사라 타원체(Vincenty) 거리와 0.5% 안에서 맞는다") {
        const auto ellipsoidal = marine::geodesy::vincenty_inverse(busan, fukuoka);
        REQUIRE(ellipsoidal);
        CHECK_THAT(marine::geodesy::haversine_distance(busan, fukuoka), WithinRel(ellipsoidal->distance, 0.005));
    }
    SECTION("대칭") {
        CHECK_THAT(marine::geodesy::haversine_distance(busan, fukuoka),
                   WithinAbs(marine::geodesy::haversine_distance(fukuoka, busan), 1e-6));
    }
    SECTION("날짜변경선을 넘는다") {
        // 경도 179 → -179는 2도 차이, 적도에서 약 222km.
        CHECK_THAT(marine::geodesy::haversine_distance({0, 179}, {0, -179}), WithinAbs(222'390, 100));
    }
    SECTION("대척점에서 NaN이 나지 않는다") {
        const double d = marine::geodesy::haversine_distance({0, 0}, {0, 180});
        CHECK_THAT(d, WithinAbs(marine::geodesy::kEarthRadiusMeters * 3.141592653589793, 1e-3));
    }
}

TEST_CASE("initial_bearing") {
    CHECK_THAT(marine::geodesy::initial_bearing({0, 0}, {1, 0}), WithinAbs(0, 1e-9));
    CHECK_THAT(marine::geodesy::initial_bearing({0, 0}, {0, 1}), WithinAbs(90, 1e-9));
    CHECK_THAT(marine::geodesy::initial_bearing({0, 0}, {-1, 0}), WithinAbs(180, 1e-9));
    CHECK_THAT(marine::geodesy::initial_bearing({0, 0}, {0, -1}), WithinAbs(270, 1e-9));
}
