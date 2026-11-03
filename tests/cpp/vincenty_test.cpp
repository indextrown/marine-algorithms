#include "marine/geodesy.hpp"

#include "geodtest.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>

using namespace marine::geodesy;
using marine::test::angle_diff;
using Catch::Matchers::WithinAbs;

TEST_CASE("vincenty_inverse: 수렴하면 GeodTest와 맞고, 실패는 대척점 근처에서만 난다") {
    const auto cases = marine::test::load_geodtest();
    REQUIRE(cases.size() == 400);

    int failures = 0;
    double worst_distance = 0, min_failed_arc = 180;
    for (const auto& c : cases) {
        const auto r = vincenty_inverse({c.lat1, c.lon1}, {c.lat2, c.lon2});
        if (!r) {
            CHECK(r.error() == GeodesyError::not_converged);
            ++failures;
            min_failed_arc = std::min(min_failed_arc, c.a12);
            continue;
        }
        worst_distance = std::max(worst_distance, std::fabs(r->distance - c.s12));
    }
    std::printf("vincenty_inverse: failures=%d, min failed a12=%.6f, worst distance error=%.3e m\n",
                failures, min_failed_arc, worst_distance);
    CHECK(worst_distance < 1e-3);
    // 실패는 보조 구 위의 호가 179도를 넘는(대척점에서 1도 안쪽) 경우에만.
    CHECK(min_failed_arc > 179);
}

TEST_CASE("vincenty_direct는 GeodTest 도착점과 맞는다") {
    const auto cases = marine::test::load_geodtest();
    REQUIRE(cases.size() == 400);

    double worst_position = 0;
    for (const auto& c : cases) {
        const auto r = vincenty_direct({c.lat1, c.lon1}, c.azi1, c.s12);
        REQUIRE(r);
        // 오차가 mm 이하라 구면 근사로 재도 충분하다.
        worst_position = std::max(worst_position, haversine_distance(r->destination, {c.lat2, c.lon2}));
    }
    std::printf("vincenty_direct: worst position error=%.3e m\n", worst_position);
    CHECK(worst_position < 1e-3);
}

TEST_CASE("vincenty 경계 경우") {
    SECTION("같은 점") {
        const auto r = vincenty_inverse({35, 129}, {35, 129});
        REQUIRE(r);
        CHECK(r->distance == 0.0);
    }
    SECTION("적도 위 1도") {
        const auto r = vincenty_inverse({0, 0}, {0, 1});
        REQUIRE(r);
        CHECK_THAT(r->distance, WithinAbs(111319.4907932736, 1e-6));
    }
    SECTION("대척점 근처에서는 not_converged") {
        const auto r = vincenty_inverse({0, 0}, {0.5, 179.7});
        REQUIRE_FALSE(r);
        CHECK(r.error() == GeodesyError::not_converged);
    }
    SECTION("정문제 후 역문제를 하면 원래 거리와 방위로 돌아온다") {
        const Coordinate busan{35.1028, 129.0403};
        const auto d = vincenty_direct(busan, 135, 500'000);
        REQUIRE(d);
        const auto i = vincenty_inverse(busan, d->destination);
        REQUIRE(i);
        CHECK_THAT(i->distance, WithinAbs(500'000, 1e-4));
        CHECK(std::fabs(angle_diff(i->initial_bearing, 135)) < 1e-8);
    }
    SECTION("편평률 0이면 구: 하버사인과 같은 거리") {
        const Ellipsoid sphere{kEarthRadiusMeters, 0};
        const auto r = vincenty_inverse({35.1028, 129.0403}, {33.5902, 130.4017}, sphere);
        REQUIRE(r);
        CHECK_THAT(r->distance, WithinAbs(haversine_distance({35.1028, 129.0403}, {33.5902, 130.4017}), 1e-6));
    }
}

TEST_CASE("vincenty: 잘못된 입력은 invalid_argument") {
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    constexpr double inf = std::numeric_limits<double>::infinity();
    const Coordinate ok{35, 129};
    const Coordinate bad[] = {
        {nan, 0}, {0, nan}, {inf, 0}, {-inf, 0}, {0, inf}, {0, -inf}, {90.000001, 0}, {-90.000001, 0},
    };

    for (const Coordinate c : bad) {
        CAPTURE(c.latitude, c.longitude);
        auto r1 = vincenty_inverse(c, ok);
        REQUIRE_FALSE(r1);
        CHECK(r1.error() == GeodesyError::invalid_argument);
        auto r2 = vincenty_inverse(ok, c);
        REQUIRE_FALSE(r2);
        CHECK(r2.error() == GeodesyError::invalid_argument);
        auto r3 = vincenty_direct(c, 0, 1000);
        REQUIRE_FALSE(r3);
        CHECK(r3.error() == GeodesyError::invalid_argument);
    }

    SECTION("정문제의 방위·거리") {
        for (const double x : {nan, inf, -inf}) {
            CHECK(vincenty_direct(ok, x, 1000).error() == GeodesyError::invalid_argument);
            CHECK(vincenty_direct(ok, 0, x).error() == GeodesyError::invalid_argument);
        }
    }
    SECTION("타원체") {
        for (const Ellipsoid e : {Ellipsoid{0, 0}, Ellipsoid{-1, 0}, Ellipsoid{nan, 0}, Ellipsoid{inf, 0},
                                  Ellipsoid{6378137, -0.1}, Ellipsoid{6378137, 1}, Ellipsoid{6378137, nan}}) {
            CAPTURE(e.semi_major_axis, e.flattening);
            CHECK(vincenty_inverse(ok, {34, 130}, e).error() == GeodesyError::invalid_argument);
            CHECK(vincenty_direct(ok, 0, 1000, e).error() == GeodesyError::invalid_argument);
        }
    }
    SECTION("경계값은 계산한다: 위도 ±90, 경도 범위 밖(유한값)") {
        CHECK(vincenty_inverse({90, 0}, {-90, 0}).has_value());
        CHECK(vincenty_inverse({0, 0}, {0, 361}).has_value());
        CHECK(vincenty_direct({-90, 0}, 0, 1000).has_value());
    }
}
