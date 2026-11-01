import MarineAlgorithms
import SwiftUI

/// m와 해리를 서로 바꿉니다.
struct DistanceUnitsExampleView: View {
    @State private var meters = 1852.0

    var body: some View {
        Form {
            Section {
                NumberField(title: "미터", value: $meters, unit: "m")
                NumberField(title: "해리", value: nauticalMiles, unit: "nm")
            } header: {
                Text("한쪽을 고치면 다른 쪽이 바뀌어요")
            } footer: {
                Text("1해리는 1852m로 정해져 있어요. 위도 1분(1/60도)의 길이에서 왔어요.")
            }

            Section("자주 쓰는 값") {
                ForEach([0.1, 1, 3, 12, 200], id: \.self) { nm in
                    ResultRow(title: Formatted.number(nm, digits: 1) + " nm",
                              value: Formatted.distance(Units.nauticalMilesToMeters(nm)))
                }
            }
        }
    }

    private var nauticalMiles: Binding<Double> {
        Binding(
            get: { Units.metersToNauticalMiles(meters) },
            set: { meters = Units.nauticalMilesToMeters($0) }
        )
    }
}

#Preview {
    NavigationStack { DistanceUnitsExampleView() }
}
