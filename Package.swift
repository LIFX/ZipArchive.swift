import PackageDescription

let package = Package(
    name: "ZipArchive",
    targets: [
        Target(name: "ZipArchive", dependencies: [.Target(name: "CZlib")]),
        Target(name: "CZlib")
    ]
)
