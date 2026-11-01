import MarineAlgorithms
import SwiftUI

/// 노트와 m/s를 서로 바꿉니다.
struct SpeedUnitsExampleView: View {
    @State private var knots = 12.0

    var body: some View {
        Form {
            Section {
                NumberField(title: "노트", value: $knots, unit: "kn")
                Slider(value: $knots, in: 0...40, step: 0.5)
                NumberField(title: "초속", value: metersPerSecond, unit: "m/s")
            } header: {
                Text("한쪽을 고치면 다른 쪽이 바뀌어요")
            } footer: {
                Text("1노트는 1시간에 1해리(1852m)를 가는 속력이에요. AIS의 SOG가 노트 단위예요.")
            }

            Section("이 속력으로") {
                ResultRow(title: "1분에", value: Formatted.distance(Units.knotsToMetersPerSecond(knots) * 60))
                ResultRow(title: "1시간에", value: Formatted.number(knots, digits: 1) + " nm")
                ResultRow(title: "시속", value: Formatted.number(Units.knotsToMetersPerSecond(knots) * 3.6, digits: 2) + " km/h")
            }
        }
    }

    private var metersPerSecond: Binding<Double> {
        Binding(
            get: { Units.knotsToMetersPerSecond(knots) },
            set: { knots = Units.metersPerSecondToKnots($0) }
        )
    }
}

#Preview {
    NavigationStack { SpeedUnitsExampleView() }
}
