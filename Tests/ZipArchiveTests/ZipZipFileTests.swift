//
//  ZipZipFileTests.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2017/07/08.
//  Copyright © 2017 yaslab. All rights reserved.
//

import Foundation
import XCTest
import ZipArchive

class ZipZipFileTests: BaseTestCase {
    
    override func setUp() {
        super.setUp()
        // Put setup code here. This method is called before the invocation of each test method in the class.
        do {
            try cleanUp()
        } catch {
            XCTFail()
        }
    }
    
    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        super.tearDown()
    }

    func testAppendEntryFromData() {
        let files: [String : Data] = [
            "test_data1.dat" : createFixedData(),
            "subdir/test_data2.dat" : createRandomData(),
            "subdir/" : createRandomData(size: 0)
        ]
        
        let archiveFile = zipDestinationDirectory + "test.zip"
        let archive = Zip(toFileAtPath: archiveFile)!
        
        for fileName in files.keys {
            let data = files[fileName]!
            let isDirectory = fileName.hasSuffix("/")
            try! archive.appendEntry(fromData: data, entryName: fileName, isDirectory: isDirectory)
        }
        
        archive.close()
        
        let ret = executeCommand(
            command: "/usr/bin/unzip",
            arguments: ["-d", unzipDestinationDirectory, archiveFile],
            workingDirectory: nil
        )
        XCTAssertEqual(0, ret)
        
        for fileName in files.keys {
            let unzipTestDataFile = unzipDestinationDirectory + fileName
            let isDirectory = fileName.hasSuffix("/")
            if isDirectory {
                var flag: ObjCBool = false
                let exist = FileManager.default.fileExists(atPath: unzipTestDataFile, isDirectory: &flag)
                XCTAssertTrue(exist)
                XCTAssertTrue(flag.boolValue)
            } else {
                let unzipTestData = try! Data(contentsOf: URL(fileURLWithPath: unzipTestDataFile))
                let data = files[fileName]!
                XCTAssertEqual(data, unzipTestData)
            }
        }
    }
    
    func testExtractToFile() {
        let files: [String : Data] = [
            "test_data1.dat" : createFixedData(),
            "subdir/test_data2.dat" : createRandomData(),
            "subdir/" : createRandomData(size: 0)
        ]
        
        let archiveFile = zipDestinationDirectory + "test.zip"
        let archive = Zip(toFileAtPath: archiveFile)!
        
        for fileName in files.keys {
            let testDataFileURL = URL(fileURLWithPath: testDataDirectory + fileName)
            let isDirectory = fileName.hasSuffix("/")
            if isDirectory {
                try! FileManager.default.createDirectory(at: testDataFileURL, withIntermediateDirectories: true, attributes: nil)
            } else {
                try! FileManager.default.createDirectory(at: testDataFileURL.deletingLastPathComponent(), withIntermediateDirectories: true, attributes: nil)
                try! files[fileName]!.write(to: testDataFileURL)
            }
            try! archive.appendEntry(fromFileAtPath: testDataFileURL.path, entryName: fileName)
        }
        
        archive.close()
        
        let ret = executeCommand(
            command: "/usr/bin/unzip",
            arguments: ["-d", unzipDestinationDirectory, archiveFile],
            workingDirectory: nil
        )
        XCTAssertEqual(0, ret)
        
        for fileName in files.keys {
            let unzipTestDataFile = unzipDestinationDirectory + fileName
            let isDirectory = fileName.hasSuffix("/")
            if isDirectory {
                var flag: ObjCBool = false
                let exist = FileManager.default.fileExists(atPath: unzipTestDataFile, isDirectory: &flag)
                XCTAssertTrue(exist)
                XCTAssertTrue(flag.boolValue)
            } else {
                let unzipTestData = try! Data(contentsOf: URL(fileURLWithPath: unzipTestDataFile))
                let data = files[fileName]!
                XCTAssertEqual(data, unzipTestData)
            }
        }
    }
    
}
