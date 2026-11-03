#include "marine/geodesy.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <cmath>
#include <limits>

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

TEST_CASE("하버사인: 잘못된 좌표는 NaN") {
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    constexpr double inf = std::numeric_limits<double>::infinity();
    const Coordinate ok{35, 129};
    const Coordinate bad[] = {
        {nan, 0}, {0, nan}, {inf, 0}, {0, -inf}, {90.000001, 0}, {-90.000001, 0},
    };
    for (const Coordinate c : bad) {
        CAPTURE(c.latitude, c.longitude);
        CHECK(std::isnan(marine::geodesy::haversine_distance(c, ok)));
        CHECK(std::isnan(marine::geodesy::haversine_distance(ok, c)));
        CHECK(std::isnan(marine::geodesy::initial_bearing(c, ok)));
        CHECK(std::isnan(marine::geodesy::initial_bearing(ok, c)));
    }
    // 극은 유효하다.
    CHECK_THAT(marine::geodesy::haversine_distance({90, 0}, {-90, 0}),
               WithinRel(marine::geodesy::kEarthRadiusMeters * 3.141592653589793, 1e-12));
}
