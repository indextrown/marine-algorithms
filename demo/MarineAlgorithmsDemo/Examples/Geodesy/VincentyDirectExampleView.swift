import MarineAlgorithms
import SwiftUI

/// Vincenty 정문제로 출발점에서 방위와 거리만큼 간 도착점을 구합니다.
struct VincentyDirectExampleView: View {
    @State private var from = Place.busan.coordinate
    @State private var bearing = 135.0
    @State private var distanceKilometers = 500.0

    private var result: Result<DirectSolution, GeodesyError> {
        Result { () throws(GeodesyError) in
            try Geodesy.vincentyDirect(from: from, bearing: bearing, distance: distanceKilometers * 1000)
        }
    }

    var body: some View {
        Form {
            CoordinateSection(title: "출발점", coordinate: $from)

            Section("이동") {
                NumberField(title: "방위", value: $bearing, unit: "°")
                Slider(value: $bearing, in: 0...360, step: 1)
                NumberField(title: "거리", value: $distanceKilometers, unit: "km")
                NumberField(title: "거리(해리)", value: nauticalMiles, unit: "nm")
            }

            switch result {
            case .success(let solution):
                Section {
                    ResultRow(title: "위도", value: Formatted.coordinate(solution.destination.latitude))
                    ResultRow(title: "경도", value: Formatted.coordinate(solution.destination.longitude))
                    ResultRow(title: "도착 방위", value: Formatted.bearing(solution.finalBearing))
                } header: {
                    Text("도착점")
                } footer: {
                    Text("대권을 따라가므로 도착 방위는 처음 방위와 달라요.")
                }
            case .failure:
                Section {
                    Label("계산이 수렴하지 않았어요(notConverged).", systemImage: "exclamationmark.triangle")
                        .foregroundStyle(.orange)
                }
            }
        }
    }

    /// km 입력과 같은 거리를 해리로 보여 주고, 해리를 고치면 km를 바꿉니다.
    private var nauticalMiles: Binding<Double> {
        Binding(
            get: { Units.metersToNauticalMiles(distanceKilometers * 1000) },
            set: { distanceKilometers = Units.nauticalMilesToMeters($0) / 1000 }
        )
    }
}

#Preview {
    NavigationStack { VincentyDirectExampleView() }
}
