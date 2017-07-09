//
//  ZipFileTests.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2017/06/14.
//  Copyright © 2017 yaslab. All rights reserved.
//

import Foundation
import XCTest
import ZipArchive

class ZipFileTests: BaseTestCase {
    
    static let allTests = [
        ("testSelfZipSelfUnzipWithFixedData", testSelfZipSelfUnzipWithFixedData),
        ("testSystemZipSelfUnzipWithFixedData", testSystemZipSelfUnzipWithFixedData),
        ("testSelfZipSystemUnzipWithFixedData", testSelfZipSystemUnzipWithFixedData),
        ("testSelfZipSelfUnzipWithRandomData", testSelfZipSelfUnzipWithRandomData),
        ("testSystemZipSelfUnzipWithRandomData", testSystemZipSelfUnzipWithRandomData),
        ("testSelfZipSystemUnzipWithRandomData", testSelfZipSystemUnzipWithRandomData)
    ]
    
    override func setUp() {
        super.setUp()
        do {
            try cleanUp()
        }
        catch { XCTFail() }
    }
    
    override func tearDown() {
        super.tearDown()
    }
    
    // MARK: Fixed Data
    
    func testSelfZipSelfUnzipWithFixedData() {
        let files: [String : Data] = [
            "test_data1.dat" : createFixedData()
        ]
        _testSelfZipSelfUnzip(files: files)
    }
    
    func testSystemZipSelfUnzipWithFixedData() {
        let files: [String : Data] = [
            "test_data1.dat" : createFixedData()
        ]
        _testSystemZipSelfUnzip(files: files)
    }
    
    func testSelfZipSystemUnzipWithFixedData() {
        let files: [String : Data] = [
            "test_data1.dat" : createFixedData()
        ]
        _testSelfZipSystemUnzip(files: files)
    }
    
    // MARK: Random Data
    
    func testSelfZipSelfUnzipWithRandomData() {
        let files: [String : Data] = [
            "test_data1.dat" : createRandomData()
        ]
        _testSelfZipSelfUnzip(files: files)
    }
    
    func testSystemZipSelfUnzipWithRandomData() {
        let files: [String : Data] = [
            "test_data1.dat" : createRandomData(),
            "test_data2.dat" : createRandomData(),
            "test_data3.dat" : createFixedData(),
            "test_data4.dat" : createFixedData(),
            "test_data5.dat" : createRandomData(),
            "test_data6.dat" : createFixedData(),
            "test_data7.dat" : createRandomData(),
            "test_data8.dat" : createFixedData()
        ]
        _testSystemZipSelfUnzip(files: files)
    }
    
    func testSelfZipSystemUnzipWithRandomData() {
        let files: [String : Data] = [
            "test_data1.dat" : createRandomData(),
            "test_data2.dat" : createRandomData(),
            "test_data3.dat" : createFixedData(),
            "test_data4.dat" : createFixedData(),
            "test_data5.dat" : createRandomData(),
            "test_data6.dat" : createFixedData(),
            "test_data7.dat" : createRandomData(),
            "test_data8.dat" : createFixedData()
        ]
        _testSelfZipSystemUnzip(files: files)
    }
    
    // MARK: Utility
    
    private func _testSelfZipSelfUnzip(files: [String : Data]) {
        let created = createFiles(files: files, baseDirectory: testDataDirectory)
        XCTAssertTrue(created)
        
        let zipFileName = "test.zip"
        let zipFilePath = zipDestinationDirectory + zipFileName
        
        do {
            try ZipFile.createFromDirectory(at: testDataDirectory, to: zipFilePath)
        } catch {
            XCTFail("\(error)")
        }
        
        do {
            try ZipFile.extractToDirectory(at: unzipDestinationDirectory, from: zipFilePath)
        } catch {
            XCTFail("\(error)")
        }
        
        for fileName in files.keys {
            let data = files[fileName]!
            let unzippedPath = unzipDestinationDirectory + fileName
            let unzippedData = try! Data(contentsOf: URL(fileURLWithPath: unzippedPath))
            XCTAssertEqual(data, unzippedData)
        }
    }
    
    private func _testSystemZipSelfUnzip(files: [String : Data]) {
        let created = createFiles(files: files, baseDirectory: testDataDirectory)
        XCTAssertTrue(created)
        
        let zipFileName = "test.zip"
        let zipFilePath = zipDestinationDirectory + zipFileName
        
        let ret = executeCommand(
            command: "/usr/bin/zip",
            arguments: [zipFilePath, "-r", "."],
            workingDirectory: testDataDirectory
        )
        XCTAssertEqual(0, ret)
        
        try! ZipFile.extractToDirectory(at: unzipDestinationDirectory, from: zipFilePath)
        
        for fileName in files.keys {
            let data = files[fileName]!
            let unzippedPath = unzipDestinationDirectory + fileName
            let unzippedData = try! Data(contentsOf: URL(fileURLWithPath: unzippedPath))
            XCTAssertEqual(data, unzippedData)
        }
    }
    
    private func _testSelfZipSystemUnzip(files: [String : Data]) {
        let created = createFiles(files: files, baseDirectory: testDataDirectory)
        XCTAssertTrue(created)
        
        let zipFileName = "test.zip"
        let zipFilePath = zipDestinationDirectory + zipFileName
        
        try! ZipFile.createFromDirectory(at: testDataDirectory, to: zipFilePath)
        
        let ret = executeCommand(
            command: "/usr/bin/unzip",
            arguments: ["-d", unzipDestinationDirectory, zipFilePath],
            workingDirectory: nil
        )
        XCTAssertEqual(0, ret)
        
        for fileName in files.keys {
            let data = files[fileName]!
            let unzippedPath = unzipDestinationDirectory + fileName
            let unzippedData = try! Data(contentsOf: URL(fileURLWithPath: unzippedPath))
            XCTAssertEqual(data, unzippedData)
        }
    }
    
}
