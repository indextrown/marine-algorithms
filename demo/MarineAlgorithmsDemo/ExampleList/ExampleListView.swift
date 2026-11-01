import SwiftUI

/// 라이브러리 예제를 묶음별 목록으로 보여 줍니다.
struct ExampleListView: View {
    var body: some View {
        NavigationStack {
            List {
                ForEach(ExampleSection.allCases, id: \.self) { section in
                    Section(section.title) {
                        ForEach(section.examples, id: \.self) { example in
                            NavigationLink(value: example) {
                                ExampleRow(example: example)
                            }
                        }
                    }
                }
            }
            .navigationTitle("MarineAlgorithms")
            .navigationDestination(for: Example.self) { example in
                example.destination
                    .navigationTitle(example.title)
                    .navigationBarTitleDisplayMode(.inline)
            }
        }
    }
}

/// 예제의 아이콘, 제목, 설명을 표시하는 행입니다.
private struct ExampleRow: View {
    let example: Example

    var body: some View {
        Label {
            VStack(alignment: .leading, spacing: 2) {
                Text(example.title)
                Text(example.subtitle)
                    .font(.caption)
                    .foregroundStyle(.secondary)
            }
        } icon: {
            Image(systemName: example.systemImageName)
                .foregroundStyle(.tint)
        }
    }
}

#Preview {
    ExampleListView()
}
