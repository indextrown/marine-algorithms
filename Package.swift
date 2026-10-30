// swift-tools-version: 6.0
import PackageDescription

let package = Package(
    name: "MarineAlgorithms",
    platforms: [.iOS(.v15), .macOS(.v13)],
    products: [
        .library(name: "MarineAlgorithms", targets: ["MarineAlgorithms"]),
    ],
    targets: [
        // C++ 코어 + C 다리층. Swift에는 marine_c.h 하나만 보인다.
        .target(
            name: "CMarineAlgorithms",
            path: ".",
            sources: ["src", "bindings/c/marine_c.cpp"],
            publicHeadersPath: "bindings/c/include",
            cxxSettings: [
                .headerSearchPath("include"),
            ]
        ),
        .target(
            name: "MarineAlgorithms",
            dependencies: ["CMarineAlgorithms"],
            path: "bindings/swift/Sources/MarineAlgorithms"
        ),
        .testTarget(
            name: "MarineAlgorithmsTests",
            dependencies: ["MarineAlgorithms"],
            path: "bindings/swift/Tests/MarineAlgorithmsTests"
        ),
    ],
    cxxLanguageStandard: .cxx2b
)
