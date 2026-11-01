import MarineAlgorithms
import SwiftUI

/// 좌표 하나를 항구 목록에서 고르거나 위도·경도로 직접 넣는 Form 묶음입니다.
struct CoordinateSection: View {
    let title: String
    @Binding var coordinate: Coordinate

    var body: some View {
        Section(title) {
            Menu {
                ForEach(Place.allCases) { place in
                    Button(place.rawValue) { coordinate = place.coordinate }
                }
            } label: {
                Label("항구에서 고르기", systemImage: "mappin.and.ellipse")
            }
            NumberField(title: "위도", value: $coordinate.latitude, unit: "°")
            NumberField(title: "경도", value: $coordinate.longitude, unit: "°")
        }
    }
}

/// 제목, 숫자 입력, 단위를 한 줄에 놓는 입력 행입니다.
struct NumberField: View {
    let title: String
    @Binding var value: Double
    var unit: String = ""

    var body: some View {
        LabeledContent(title) {
            HStack(spacing: 4) {
                TextField(title, value: $value, format: .number.precision(.fractionLength(0...6)))
                    .keyboardType(.numbersAndPunctuation)
                    .multilineTextAlignment(.trailing)
                    .monospacedDigit()
                Text(unit)
                    .foregroundStyle(.secondary)
            }
        }
    }
}
