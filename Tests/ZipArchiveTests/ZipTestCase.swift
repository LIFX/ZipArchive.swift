//
//  ZipTestCase.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2017/06/10.
//
//

import Foundation
import XCTest
import ZipArchive

class ZipTests: BaseTestCase {
    
    static let allTests = [
        ("test圧縮ファイル", test圧縮ファイル),
        ("test圧縮ファイルのバッファサイズ", test圧縮ファイルのバッファサイズ)
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
    
    func test圧縮ファイル() {
        let files: [String : Data] = [
            "test_data.dat" : createFixedData()
        ]
        
        let archiveFile = zipDestinationDirectory + "test.zip"
        //let archive = try! ZipArchive(path: archiveFile, mode: .create)
        do {
        let archive = Zip(toFileAtPath: archiveFile)!
        
        for fileName in files.keys {
            let entry = archive.appendEntry(fileName, level: .default)
            //let stream = try! entry.open()
            let data = files[fileName]!
            let length = data.count
            data.withUnsafeBytes { (buffer) -> Void in
                _ = entry.write(buffer, count: length)
            }
//            data.withUnsafeBytes { (buffer) -> Void in
//                _ = stream.write(buffer: buffer, maxLength: 0) // flush
//            }
//            stream.close()
            
            entry.close()
        }
        
//        archive.dispose()
        archive.close()
        }
        
        let ret = executeCommand(
            command: "/usr/bin/unzip",
            arguments: ["-d", unzipDestinationDirectory, archiveFile],
            workingDirectory: nil
        )
        XCTAssertEqual(0, ret)
        
        for fileName in files.keys {
            let unzipTestDataFile = unzipDestinationDirectory + fileName
            let unzipTestData = try! Data(contentsOf: URL(fileURLWithPath: unzipTestDataFile))
            let data = files[fileName]!
            XCTAssertEqual(data, unzipTestData)
        }
    }
    
    func test圧縮ファイルのバッファサイズ() {
        let files: [String : Data] = [
            "test_data1.dat" : createFixedData(size: DefaultBufferSize - 1),
            "test_data2.dat" : createFixedData(size: DefaultBufferSize),
            "test_data3.dat" : createFixedData(size: DefaultBufferSize + 1),
            "test_data4.dat" : createRandomData(size: DefaultBufferSize * 2 - 1),
            "test_data5.dat" : createRandomData(size: DefaultBufferSize * 2),
            "test_data6.dat" : createRandomData(size: DefaultBufferSize * 2 + 1),
            "test_data7.dat" : createFixedData(size: DefaultBufferSize * 3 - 1),
            "test_data8.dat" : createRandomData(size: DefaultBufferSize * 3),
            "test_data9.dat" : createFixedData(size: DefaultBufferSize * 3 + 1),
        ]
        
        //let created = createFiles(files: files, baseDirectory: testDataDirectory)
        //XCTAssertTrue(created)
        
        let archiveFile = zipDestinationDirectory + "test.zip"
        //let archive = try! ZipArchive(path: archiveFile, mode: .create)
        do {
        let archive = Zip(toFileAtPath: archiveFile)!
        
        for fileName in files.keys {
            //let testDataFilePath = testDataDirectory + fileName
            //_ = try! archive.createEntryFrom(filePath: testDataFilePath, entryName: fileName)
            
            
            let entry = archive.appendEntry(fileName, level: .optimal)
            let data = files[fileName]!
            let length = data.count
            data.withUnsafeBytes { (buffer) -> Void in
                _ = entry.write(buffer, count: length)
            }
            entry.close()
        }
        
        //archive.dispose()
        archive.close()
        }
        
        let ret = executeCommand(
            command: "/usr/bin/unzip",
            arguments: ["-d", unzipDestinationDirectory, archiveFile],
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

