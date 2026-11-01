import MarineAlgorithms

/// 예제에서 고를 수 있는 항구 좌표입니다.
enum Place: String, CaseIterable, Identifiable {
    case busan = "부산"
    case fukuoka = "후쿠오카"
    case tokyo = "도쿄"
    case shanghai = "상하이"
    case singapore = "싱가포르"
    case losAngeles = "로스앤젤레스"

    var id: Self { self }

    var coordinate: Coordinate {
        switch self {
        case .busan: Coordinate(latitude: 35.1028, longitude: 129.0403)
        case .fukuoka: Coordinate(latitude: 33.5902, longitude: 130.4017)
        case .tokyo: Coordinate(latitude: 35.6528, longitude: 139.7700)
        case .shanghai: Coordinate(latitude: 31.2304, longitude: 121.4737)
        case .singapore: Coordinate(latitude: 1.2640, longitude: 103.8400)
        case .losAngeles: Coordinate(latitude: 33.7405, longitude: -118.2775)
        }
    }
}
