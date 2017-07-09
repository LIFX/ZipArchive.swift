# ZipArchive.swift

[![Build Status](https://travis-ci.org/yaslab/ZipArchive.swift.svg?branch=master)](https://travis-ci.org/yaslab/ZipArchive.swift)
[![codecov](https://codecov.io/gh/yaslab/ZipArchive.swift/branch/master/graph/badge.svg)](https://codecov.io/gh/yaslab/ZipArchive.swift)

Zip archiving library written in Swift and C.

This library inspired by System.IO.Compression namespace of the .NET Framework.

## Usage

### Unzip

```swift
import ZipArchive

let sourceFile = "/path/to/archive.zip"
let destinationDirectory = "/path/to/directory"
try! ZipFile.extractToDirectory(sourceFile, destinationDirectoryName: destinationDirectory)
```

### Zip

```swift
import ZipArchive

let sourceDirectory = "/path/to/directory"
let destinationFile = "/path/to/archive.zip"
try! ZipFile.createFromDirectory(sourceDirectory, destinationArchiveFileName: destinationFile)
```

### Enumerate files in zip file

```swift
import ZipArchive

let archiveFile = "/path/to/archive.zip"
let archive = ZipArchive(path: archiveFile, mode: .Read)!
defer { archive.dispose() }

for entry in archive.entries {
    print("\(entry.fullName)")
}
```

## Installation

### CocoaPods

TBD

### Carthage

```
github "yaslab/ZipArchive.swift" ~> 0.1
```

### Swift Package Manager

```
.package(url: "https://github.com/yaslab/ZipArchive.swift.git", from: "0.1.0")
```

## License

ZipArchive.swift is licensed under the [MIT license](https://github.com/yaslab/ZipArchive.swift/blob/master/LICENSE).
