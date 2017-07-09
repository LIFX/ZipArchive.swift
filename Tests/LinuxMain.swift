import XCTest
@testable import ZipArchiveTests

XCTMain([
    testCase(UnzipTests.allTests),
    testCase(ZipFilePasswordTests.allTests),
    testCase(ZipFileTests.allTests),
    testCase(ZipStreamTests.allTests),
    testCase(ZipTests.allTests)
])
