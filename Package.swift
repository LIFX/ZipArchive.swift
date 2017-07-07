// swift-tools-version:4.0

import PackageDescription

let package = Package(
    name: "ZipArchive",
    targets: [
        .target(name: "ZipArchive", dependencies: ["CoreZipArchive"]),
        .target(name: "CoreZipArchive"),
        .testTarget(name: "ZipArchiveTests", dependencies: ["ZipArchive"])
    ],
    swiftLanguageVersions: [4]
)
