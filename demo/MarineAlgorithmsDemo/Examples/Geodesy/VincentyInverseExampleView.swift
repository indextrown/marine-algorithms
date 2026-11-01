import MarineAlgorithms
import SwiftUI

/// Vincenty 역문제로 WGS84 타원체 위 두 점 사이 거리와 방위를 구하고 하버사인과 비교합니다.
struct VincentyInverseExampleView: View {
    @State private var from = Place.busan.coordinate
    @State private var to = Place.losAngeles.coordinate

    private var result: Result<InverseSolution, GeodesyError> {
        Result { () throws(GeodesyError) in try Geodesy.vincentyInverse(from: from, to: to) }
    }

    var body: some View {
        Form {
            CoordinateSection(title: "출발점", coordinate: $from)
            CoordinateSection(title: "도착점", coordinate: $to)

            switch result {
            case .success(let solution):
                let haversine = Geodesy.haversineDistance(from: from, to: to)
                Section("결과") {
                    ResultRow(title: "거리", value: Formatted.distance(solution.distance))
                    ResultRow(title: "처음 방위", value: Formatted.bearing(solution.initialBearing))
                    ResultRow(title: "마지막 방위", value: Formatted.bearing(solution.finalBearing))
                }
                Section {
                    ResultRow(title: "하버사인 거리", value: Formatted.distance(haversine))
                    ResultRow(title: "차이", value: Formatted.distance(abs(haversine - solution.distance)))
                    if solution.distance > 0 {
                        ResultRow(title: "차이 비율",
                                  value: Formatted.number(abs(haversine - solution.distance) / solution.distance * 100,
                                                          digits: 3) + " %")
                    }
                } header: {
                    Text("하버사인과 비교")
                } footer: {
                    Text("Vincenty는 지구를 WGS84 타원체로 보고 0.1mm 안쪽까지 맞춰요.")
                }
            case .failure:
                Section {
                    Label("두 점이 거의 대척점이라 수렴하지 않았어요(notConverged).",
                          systemImage: "exclamationmark.triangle")
                        .foregroundStyle(.orange)
                }
            }
        }
    }
}

#Preview {
    NavigationStack { VincentyInverseExampleView() }
}
