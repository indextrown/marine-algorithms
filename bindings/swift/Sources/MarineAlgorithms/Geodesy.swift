import CMarineAlgorithms

/// 위도·경도(도 단위).
public struct Coordinate: Hashable, Sendable {
    public var latitude: Double
    public var longitude: Double

    public init(latitude: Double, longitude: Double) {
        self.latitude = latitude
        self.longitude = longitude
    }

    init(_ c: ma_coordinate) {
        self.init(latitude: c.latitude, longitude: c.longitude)
    }

    var c: ma_coordinate { ma_coordinate(latitude: latitude, longitude: longitude) }
}

/// 회전 타원체. 장반경 a(m)와 편평률 f.
public struct Ellipsoid: Hashable, Sendable {
    public var semiMajorAxis: Double
    public var flattening: Double

    public init(semiMajorAxis: Double, flattening: Double) {
        self.semiMajorAxis = semiMajorAxis
        self.flattening = flattening
    }

    /// GPS가 쓰는 WGS84 타원체.
    public static let wgs84: Ellipsoid = {
        let e = ma_wgs84()
        return Ellipsoid(semiMajorAxis: e.semi_major_axis, flattening: e.flattening)
    }()

    var c: ma_ellipsoid { ma_ellipsoid(semi_major_axis: semiMajorAxis, flattening: flattening) }
}

/// 역문제의 답. 방위는 진북 기준 시계 방향 [0, 360).
public struct InverseSolution: Hashable, Sendable {
    /// 측지선 길이(m)
    public var distance: Double
    /// 출발점에서의 방위
    public var initialBearing: Double
    /// 도착점에서의 방위(진행 방향)
    public var finalBearing: Double

    init(_ r: ma_inverse) {
        distance = r.distance
        initialBearing = r.initial_bearing
        finalBearing = r.final_bearing
    }
}

/// 정문제의 답.
public struct DirectSolution: Hashable, Sendable {
    public var destination: Coordinate
    /// 도착점에서의 방위(진행 방향) [0, 360)
    public var finalBearing: Double

    init(_ r: ma_direct) {
        destination = Coordinate(r.destination)
        finalBearing = r.final_bearing
    }
}

public enum GeodesyError: Error, Hashable, Sendable {
    /// 반복 계산이 수렴하지 않았다. Vincenty 역문제에서 두 점이 거의 대척점일 때 생긴다.
    case notConverged

    init(_ status: ma_status) {
        // MA_INVALID_ARGUMENT는 out을 항상 넘기므로 여기까지 오지 않는다.
        precondition(status == MA_NOT_CONVERGED, "unexpected ma_status \(status.rawValue)")
        self = .notConverged
    }
}

public enum Geodesy {
    // MARK: 구면 (하버사인)

    /// 대권 거리(m). 하버사인 공식.
    public static func haversineDistance(from: Coordinate, to: Coordinate) -> Double {
        ma_haversine_distance(from.c, to.c)
    }

    /// 처음 방위(도, 진북 기준 시계 방향 [0, 360)).
    public static func initialBearing(from: Coordinate, to: Coordinate) -> Double {
        ma_initial_bearing(from.c, to.c)
    }

    // MARK: 타원체 (Vincenty 1975)

    /// 두 점 사이의 측지선 거리와 방위. 대척점 근처에서 수렴하지 않으면 `.notConverged`.
    public static func vincentyInverse(
        from: Coordinate, to: Coordinate, ellipsoid: Ellipsoid = .wgs84
    ) throws(GeodesyError) -> InverseSolution {
        var out = ma_inverse()
        let status = ma_vincenty_inverse(from.c, to.c, ellipsoid.c, &out)
        guard status == MA_OK else { throw GeodesyError(status) }
        return InverseSolution(out)
    }

    /// 출발점에서 `bearing`(도) 방향으로 `distance`(m)만큼 간 도착점.
    public static func vincentyDirect(
        from: Coordinate, bearing: Double, distance: Double, ellipsoid: Ellipsoid = .wgs84
    ) throws(GeodesyError) -> DirectSolution {
        var out = ma_direct()
        let status = ma_vincenty_direct(from.c, bearing, distance, ellipsoid.c, &out)
        guard status == MA_OK else { throw GeodesyError(status) }
        return DirectSolution(out)
    }
}
