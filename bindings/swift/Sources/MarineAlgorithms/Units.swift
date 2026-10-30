import CMarineAlgorithms

/// 항해 단위 변환. 해리(nm) = 1852m, 노트(kn) = 1시간에 1해리.
public enum Units {
    /// m → 해리
    public static func metersToNauticalMiles(_ meters: Double) -> Double {
        ma_meters_to_nautical_miles(meters)
    }

    /// 해리 → m
    public static func nauticalMilesToMeters(_ nauticalMiles: Double) -> Double {
        ma_nautical_miles_to_meters(nauticalMiles)
    }

    /// 노트 → m/s
    public static func knotsToMetersPerSecond(_ knots: Double) -> Double {
        ma_knots_to_meters_per_second(knots)
    }

    /// m/s → 노트
    public static func metersPerSecondToKnots(_ metersPerSecond: Double) -> Double {
        ma_meters_per_second_to_knots(metersPerSecond)
    }
}
