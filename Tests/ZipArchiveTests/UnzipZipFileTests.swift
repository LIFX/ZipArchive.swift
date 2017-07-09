//
//  UnzipZipFileTests.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2017/07/08.
//  Copyright © 2017 yaslab. All rights reserved.
//

import Foundation
import XCTest
import ZipArchive

class UnzipZipFileTests: BaseTestCase {
    
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

    func testExtractToData() {
        let files: [String : Data] = [
            "test_data1.dat" : createFixedData(),
            "subdir/test_data2.dat" : createRandomData(),
            "subdir/" : createRandomData(size: 0)
        ]
        
        let created = createFiles(files: files, baseDirectory: testDataDirectory)
        XCTAssertTrue(created)
        
        let archiveFile = zipDestinationDirectory + "test.zip"
        let ret = executeCommand(
            command: "/usr/bin/zip",
            arguments: [archiveFile, "-r", "."],
            workingDirectory: testDataDirectory
        )
        XCTAssertEqual(0, ret)
        
        let archive = Unzip(fileAtPath: archiveFile)!
        
        var count = 0
        for entry in archive {
            let data = try! entry.extractToData()
            let testData = files[entry.name]!
            XCTAssertEqual(data, testData)
            count += 1
        }
        
        archive.close()
        
        XCTAssertEqual(files.count, count)
    }
    
    func testExtractToDataWithPassword() {
        let files: [String : Data] = [
            "test_data1.dat" : createFixedData(),
            "subdir/test_data2.dat" : createRandomData(),
            "subdir/" : createRandomData(size: 0)
        ]
        
        let created = createFiles(files: files, baseDirectory: testDataDirectory)
        XCTAssertTrue(created)
        
        let archiveFile = zipDestinationDirectory + "test.zip"
        let password = "abcde"
        
        let ret = executeCommand(
            command: "/usr/bin/zip",
            arguments: ["-P", password, archiveFile, "-r", "."],
            workingDirectory: testDataDirectory
        )
        XCTAssertEqual(0, ret)
        
        let archive = Unzip(fileAtPath: archiveFile)!
        
        var count = 0
        for entry in archive {
            let data = try! entry.extractToData(password: password)
            let testData = files[entry.name]!
            XCTAssertEqual(data, testData)
            count += 1
        }
        
        archive.close()
        
        XCTAssertEqual(files.count, count)
    }

    func testExtractToFile() {
        let files: [String : Data] = [
            "test_data1.dat" : createFixedData(),
            "subdir/test_data2.dat" : createRandomData(),
            "subdir/" : createRandomData(size: 0)
        ]
        
        let created = createFiles(files: files, baseDirectory: testDataDirectory)
        XCTAssertTrue(created)
        
        let archiveFile = zipDestinationDirectory + "test.zip"
        let ret = executeCommand(
            command: "/usr/bin/zip",
            arguments: [archiveFile, "-r", "."],
            workingDirectory: testDataDirectory
        )
        XCTAssertEqual(0, ret)
        
        let archive = Unzip(fileAtPath: archiveFile)!
        
        var count = 0
        for entry in archive {
            let unzippedPath = unzipDestinationDirectory + entry.name
            try! entry.extract(toFileAtPath: unzippedPath, overwrite: false)
            if entry.isDirectory {
                var isDirectory: ObjCBool = false
                let exist = FileManager.default.fileExists(atPath: unzippedPath, isDirectory: &isDirectory)
                XCTAssertTrue(isDirectory.boolValue)
                XCTAssertTrue(exist)
            } else {
                let unzippedData = try! Data(contentsOf: URL(fileURLWithPath: unzippedPath))
                let testData = files[entry.name]!
                XCTAssertEqual(unzippedData, testData)
            }
            count += 1
        }
        
        archive.close()
        
        XCTAssertEqual(files.count, count)
    }
    
}
