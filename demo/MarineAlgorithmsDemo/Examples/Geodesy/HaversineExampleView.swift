import MarineAlgorithms
import SwiftUI

/// 하버사인 공식으로 구 위의 대권 거리와 처음 방위를 구합니다.
struct HaversineExampleView: View {
    @State private var from = Place.busan.coordinate
    @State private var to = Place.fukuoka.coordinate

    private var distance: Double { Geodesy.haversineDistance(from: from, to: to) }
    private var bearing: Double { Geodesy.initialBearing(from: from, to: to) }

    var body: some View {
        Form {
            CoordinateSection(title: "출발점", coordinate: $from)
            CoordinateSection(title: "도착점", coordinate: $to)

            Section {
                ResultRow(title: "거리", value: Formatted.distance(distance))
                ResultRow(title: "거리(해리)",
                          value: Formatted.number(Units.metersToNauticalMiles(distance), digits: 3) + " nm")
                ResultRow(title: "처음 방위", value: Formatted.bearing(bearing))
            } header: {
                Text("결과")
            } footer: {
                Text("지구를 반지름 6,371km인 구로 보고 계산해요. 실제 타원체 거리와 최대 약 0.5% 차이가 나요.")
            }
        }
    }
}

#Preview {
    NavigationStack { HaversineExampleView() }
}
