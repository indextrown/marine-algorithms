import Testing
@testable import MarineAlgorithms

// 바인딩이 제대로 이어졌는지만 본다. 정확도는 C++ 테스트에서 본다.
struct UnitsTests {
    @Test func nauticalMiles() {
        #expect(Units.nauticalMilesToMeters(1) == 1852)
        #expect(Units.metersToNauticalMiles(1852) == 1)
    }

    @Test func knots() {
        #expect(abs(Units.knotsToMetersPerSecond(12) - 6.173333333333) < 1e-9)
        #expect(abs(Units.metersPerSecondToKnots(Units.knotsToMetersPerSecond(12)) - 12) < 1e-12)
    }
}
