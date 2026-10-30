#pragma once

// 측지선 테스트 데이터(tests/data/geodtest-sample.dat) 읽기.

#include <cmath>
#include <fstream>
#include <string>
#include <vector>

namespace marine::test {

struct GeodTestCase {
    double lat1, lon1, azi1;
    double lat2, lon2, azi2;
    double s12; ///< 측지선 길이(m)
    double a12; ///< 보조 구 위의 호 길이(도). 180에 가까울수록 대척점에 가깝다.
};

inline std::vector<GeodTestCase> load_geodtest() {
    std::ifstream in(std::string(MARINE_TEST_DATA_DIR) + "/geodtest-sample.dat");
    std::vector<GeodTestCase> cases;
    GeodTestCase c{};
    double m12, S12;
    while (in >> c.lat1 >> c.lon1 >> c.azi1 >> c.lat2 >> c.lon2 >> c.azi2 >> c.s12 >> c.a12 >> m12 >> S12)
        cases.push_back(c);
    return cases;
}

/// 두 각(도)의 차이 [-180, 180].
inline double angle_diff(double a, double b) { return std::remainder(a - b, 360.0); }

} // namespace marine::test
