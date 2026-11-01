import MarineAlgorithms
import SwiftUI

/// 대척점 근처에서 Vincenty 역문제가 실패하는 모습과 하버사인으로 대신하는 방법을 보여 줍니다.
struct AntipodalExampleView: View {
    @State private var from = Coordinate(latitude: 0, longitude: 0)
    @State private var to = Coordinate(latitude: 0.5, longitude: 179.7)

    private var distance: (meters: Double, method: String, failed: Bool) {
        do throws(GeodesyError) {
            return (try Geodesy.vincentyInverse(from: from, to: to).distance, "Vincenty", false)
        } catch {
            return (Geodesy.haversineDistance(from: from, to: to), "하버사인(대신 계산)", true)
        }
    }

    var body: some View {
        Form {
            Section {
                Text("대척점은 지구 반대편의 점이에요. 두 점이 거의 대척점이면 가는 길이 여러 갈래라 Vincenty 반복 계산이 답을 하나로 좁히지 못해요.")
            }

            CoordinateSection(title: "출발점", coordinate: $from)
            CoordinateSection(title: "도착점", coordinate: $to)

            Section {
                if distance.failed {
                    Label("Vincenty: notConverged", systemImage: "xmark.octagon")
                        .foregroundStyle(.orange)
                } else {
                    Label("Vincenty: 수렴", systemImage: "checkmark.circle")
                        .foregroundStyle(.green)
                }
                ResultRow(title: "거리", value: Formatted.distance(distance.meters))
                ResultRow(title: "계산 방법", value: distance.method)
            } header: {
                Text("결과")
            } footer: {
                Text("도착점 경도를 179° 아래로 낮추면 Vincenty가 다시 수렴해요.")
            }

            Section("코드") {
                Text("""
                do {
                    d = try Geodesy.vincentyInverse(from: a, to: b).distance
                } catch .notConverged {
                    d = Geodesy.haversineDistance(from: a, to: b)
                }
                """)
                .font(.system(.footnote, design: .monospaced))
            }
        }
    }
}

#Preview {
    NavigationStack { AntipodalExampleView() }
}
