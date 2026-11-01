import SwiftUI

/// 예제 목록의 묶음입니다.
enum ExampleSection: CaseIterable {
    case geodesy
    case units

    var title: String {
        switch self {
        case .geodesy: "측지 계산"
        case .units: "단위 변환"
        }
    }

    var examples: [Example] {
        Example.allCases.filter { $0.section == self }
    }
}

/// 예제 목록과 각 예제 화면을 연결하는 목적지입니다.
enum Example: CaseIterable, Hashable {
    case haversine
    case vincentyInverse
    case vincentyDirect
    case antipodal
    case distanceUnits
    case speedUnits

    var section: ExampleSection {
        switch self {
        case .haversine, .vincentyInverse, .vincentyDirect, .antipodal: .geodesy
        case .distanceUnits, .speedUnits: .units
        }
    }

    /// 목록과 상세 화면에 표시할 제목입니다.
    var title: String {
        switch self {
        case .haversine: "하버사인 거리"
        case .vincentyInverse: "Vincenty 역문제"
        case .vincentyDirect: "Vincenty 정문제"
        case .antipodal: "대척점에서 실패하기"
        case .distanceUnits: "거리: m ↔ 해리"
        case .speedUnits: "속력: m/s ↔ 노트"
        }
    }

    /// 목록에서 제목 아래에 표시할 설명입니다.
    var subtitle: String {
        switch self {
        case .haversine: "구 위의 두 점 사이 대권 거리와 처음 방위"
        case .vincentyInverse: "WGS84 타원체에서 두 점 사이 거리와 방위"
        case .vincentyDirect: "출발점, 방위, 거리로 도착점 구하기"
        case .antipodal: "notConverged를 받아 하버사인으로 대신하기"
        case .distanceUnits: "1해리 = 1852m"
        case .speedUnits: "1노트 = 1시간에 1해리"
        }
    }

    /// 목록 아이콘으로 쓸 SF Symbols 이름입니다.
    var systemImageName: String {
        switch self {
        case .haversine: "globe"
        case .vincentyInverse: "ruler"
        case .vincentyDirect: "location.north.line"
        case .antipodal: "exclamationmark.triangle"
        case .distanceUnits: "arrow.left.and.right"
        case .speedUnits: "speedometer"
        }
    }

    @MainActor @ViewBuilder
    var destination: some View {
        switch self {
        case .haversine: HaversineExampleView()
        case .vincentyInverse: VincentyInverseExampleView()
        case .vincentyDirect: VincentyDirectExampleView()
        case .antipodal: AntipodalExampleView()
        case .distanceUnits: DistanceUnitsExampleView()
        case .speedUnits: SpeedUnitsExampleView()
        }
    }
}
