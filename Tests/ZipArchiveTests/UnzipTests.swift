//
//  UnzipTests.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2016/01/31.
//  Copyright © 2016 yaslab. All rights reserved.
//

import Foundation
import XCTest
import ZipArchive

class UnzipTests: BaseTestCase {
    
    static let allTests = [
        ("test解凍ファイル", test解凍ファイル),
        ("test解凍ファイルとディレクトリ", test解凍ファイルとディレクトリ),
        ("test解凍ファイルのバッファサイズ", test解凍ファイルのバッファサイズ)
    ]

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

    func test解凍ファイル() {
        let files: [String : Data] = [
            "test_data.dat" : createFixedData()
        ]

        let created = createFiles(files: files, baseDirectory: testDataDirectory)
        XCTAssertTrue(created)
        
        let archiveFile = zipDestinationDirectory + "test.zip"
        let fileArgs =  files.map { (pair) -> String in pair.0 }
        let ret = executeCommand(
            command: "/usr/bin/zip",
            arguments: [archiveFile] + fileArgs,
            workingDirectory: testDataDirectory
        )
        XCTAssertEqual(0, ret)
        
        let archive = Unzip(fileAtPath: archiveFile)!
        //let archive = try! ZipArchive(path: archiveFile, mode: .read)
        
        var count = 0
        for entry in archive {
            entry.open()
//            let stream = try! entry.open()
            var data = Data(count: Int(entry.size))
            let length = data.count
            data.withUnsafeMutableBytes { (buffer) -> Void in
//                _ = stream.read(buffer: buffer, maxLength: length)
                _ = entry.read(buffer, count: length)
            }
            entry.close()
            let testData = files[entry.name]!
            XCTAssertEqual(data, testData)
            count += 1
        }

        //archive.dispose()
        
        XCTAssertEqual(files.count, count)
    }
    
    func test解凍ファイルとディレクトリ() {
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
        //let archive = try! ZipArchive(path: archiveFile, mode: .read)
        
        var count = 0
        for entry in archive {
            entry.open()
            //let stream = try! entry.open()
            var data = Data(count: Int(entry.size))
            let length = data.count
            data.withUnsafeMutableBytes { (buffer) -> Void in
                //_ = stream.read(buffer: buffer, maxLength: length)
                _ = entry.read(buffer, count: length)
            }
            entry.close()
            let testData = files[entry.name]!
            XCTAssertEqual(data, testData)
            count += 1
        }

        //archive.dispose()
        
        XCTAssertEqual(files.count, count)
    }

    func test解凍ファイルのバッファサイズ() {
        let files: [String : Data] = [
            "test_data1.dat" : createRandomData(size: DefaultBufferSize - 1),
            "test_data2.dat" : createRandomData(size: DefaultBufferSize),
            "test_data3.dat" : createRandomData(size: DefaultBufferSize + 1),
            "test_data4.dat" : createFixedData(size: DefaultBufferSize * 2 - 1),
            "test_data5.dat" : createFixedData(size: DefaultBufferSize * 2),
            "test_data6.dat" : createFixedData(size: DefaultBufferSize * 2 + 1),
            "test_data7.dat" : createRandomData(size: DefaultBufferSize * 3 - 1),
            "test_data8.dat" : createFixedData(size: DefaultBufferSize * 3),
            "test_data9.dat" : createRandomData(size: DefaultBufferSize * 3 + 1),
        ]
        
        let created = createFiles(files: files, baseDirectory: testDataDirectory)
        XCTAssertTrue(created)
        
        let archiveFile = zipDestinationDirectory + "test.zip"
        let fileArgs = files.map { (pair) -> String in pair.0 }
        let ret = executeCommand(
            command: "/usr/bin/zip",
            arguments: [archiveFile] + fileArgs,
            workingDirectory: testDataDirectory
        )
        XCTAssertEqual(0, ret)
        
        let archive = Unzip(fileAtPath: archiveFile)!
        //let archive = try! ZipArchive(path: archiveFile, mode: .read)
        
        var count = 0
        for entry in archive {
            entry.open()
            //let stream = try! entry.open()
            var data = Data(count: Int(entry.size))
            let length = data.count
            data.withUnsafeMutableBytes { (buffer) -> Void in
                _ = entry.read(buffer, count: length)
            }
            entry.close()
            let testData = files[entry.name]!
            XCTAssertEqual(data, testData)
            count += 1
        }

        //archive.dispose()
        
        XCTAssertEqual(files.count, count)
    }
    
}
