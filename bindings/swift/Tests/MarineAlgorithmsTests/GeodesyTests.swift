import Testing
@testable import MarineAlgorithms

// 바인딩이 제대로 이어졌는지만 본다. 정확도는 C++ 테스트에서 본다.
struct GeodesyTests {
    let busan = Coordinate(latitude: 35.1028, longitude: 129.0403)
    let fukuoka = Coordinate(latitude: 33.5902, longitude: 130.4017)

    // MARK: 하버사인

    @Test func samePointIsZero() {
        #expect(Geodesy.haversineDistance(from: busan, to: busan) == 0)
    }

    @Test func busanToFukuoka() {
        let distance = Geodesy.haversineDistance(from: busan, to: fukuoka)
        #expect(abs(distance - 208_000) < 2_000)
    }

    @Test func bearingDueEast() {
        let a = Coordinate(latitude: 0, longitude: 0)
        let b = Coordinate(latitude: 0, longitude: 1)
        #expect(abs(Geodesy.initialBearing(from: a, to: b) - 90) < 1e-9)
    }

    // MARK: WGS84

    @Test func wgs84() {
        #expect(Ellipsoid.wgs84.semiMajorAxis == 6_378_137)
        #expect(Ellipsoid.wgs84.flattening == 1 / 298.257223563)
    }

    // MARK: Vincenty

    @Test func vincentyCloseToHaversine() throws {
        // 타원체와 구의 차이는 0.5% 안쪽이다.
        let v = try Geodesy.vincentyInverse(from: busan, to: fukuoka)
        let h = Geodesy.haversineDistance(from: busan, to: fukuoka)
        #expect(abs(v.distance - h) / v.distance < 0.005)
    }

    @Test func vincentyEquatorOneDegree() throws {
        let r = try Geodesy.vincentyInverse(
            from: Coordinate(latitude: 0, longitude: 0),
            to: Coordinate(latitude: 0, longitude: 1))
        #expect(abs(r.distance - 111_319.4907932736) < 1e-6)
        #expect(abs(r.initialBearing - 90) < 1e-12)
    }

    @Test func vincentyThrowsNearAntipode() {
        #expect(throws: GeodesyError.notConverged) {
            try Geodesy.vincentyInverse(
                from: Coordinate(latitude: 0, longitude: 0),
                to: Coordinate(latitude: 0.5, longitude: 179.7))
        }
    }

    @Test func vincentyDirectRoundTrip() throws {
        let d = try Geodesy.vincentyDirect(from: busan, bearing: 135, distance: 500_000)
        let i = try Geodesy.vincentyInverse(from: busan, to: d.destination)
        #expect(abs(i.distance - 500_000) < 1e-3)
        #expect(abs(i.initialBearing - 135) < 1e-8)
    }

    @Test func vincentyCustomEllipsoid() throws {
        // 편평률 0이면 구. 하버사인과 같은 반지름을 주면 거의 같은 거리가 나온다.
        let sphere = Ellipsoid(semiMajorAxis: 6_371_008.8, flattening: 0)
        let v = try Geodesy.vincentyInverse(from: busan, to: fukuoka, ellipsoid: sphere)
        let h = Geodesy.haversineDistance(from: busan, to: fukuoka)
        #expect(abs(v.distance - h) < 1e-6)
    }
}
