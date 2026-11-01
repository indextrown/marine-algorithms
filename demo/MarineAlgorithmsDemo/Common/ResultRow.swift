import SwiftUI

/// 계산 결과 한 줄입니다.
struct ResultRow: View {
    let title: String
    let value: String

    var body: some View {
        LabeledContent(title) {
            Text(value)
                .monospacedDigit()
                .foregroundStyle(.primary)
                .textSelection(.enabled)
        }
    }
}

/// 결과 값을 화면에 맞게 문자열로 바꿉니다.
enum Formatted {
    static func number(_ value: Double, digits: Int) -> String {
        value.formatted(.number.precision(.fractionLength(digits)))
    }

    static func distance(_ meters: Double) -> String {
        meters >= 1000 ? number(meters / 1000, digits: 3) + " km" : number(meters, digits: 3) + " m"
    }

    static func bearing(_ degrees: Double) -> String {
        number(degrees, digits: 4) + "°"
    }

    static func coordinate(_ degrees: Double) -> String {
        number(degrees, digits: 6) + "°"
    }
}
