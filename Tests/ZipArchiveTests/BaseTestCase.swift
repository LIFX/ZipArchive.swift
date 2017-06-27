//
//  BaseTestCase.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2016/01/31.
//  Copyright © 2016 yaslab. All rights reserved.
//

import Foundation
import XCTest

class BaseTestCase: XCTestCase {

    let fileManager = FileManager.default
    let testBaseDirectory = "/tmp/ZipArchiveTest/"
    var testDataDirectory: String { return testBaseDirectory + "ZipArchiveData/" }
    var zipDestinationDirectory: String { return  testBaseDirectory + "Zip/" }
    var unzipDestinationDirectory: String { return  testBaseDirectory + "Unzip/" }
    
    let bufferSize = 8192
    
    override func setUp() {
        super.setUp()
        
        #if os(Linux)
            srand(UInt32(time(nil)))
        #endif
    }
    
    func cleanUp() throws {
        if fileManager.fileExists(atPath: testBaseDirectory) {
            try fileManager.removeItem(atPath: testBaseDirectory)
        }
        try fileManager.createDirectory(atPath: testBaseDirectory, withIntermediateDirectories: false, attributes: nil)
        try fileManager.createDirectory(atPath: testDataDirectory, withIntermediateDirectories: false, attributes: nil)
        try fileManager.createDirectory(atPath: zipDestinationDirectory, withIntermediateDirectories: false, attributes: nil)
        try fileManager.createDirectory(atPath: unzipDestinationDirectory, withIntermediateDirectories: false, attributes: nil)
    }
    
    func executeCommand(command: String, arguments: [String], workingDirectory: String?) -> Int32 {
        var success: Bool
        let prevDirectory = fileManager.currentDirectoryPath
        if let workingDirectory = workingDirectory {
            success = fileManager.changeCurrentDirectoryPath(workingDirectory)
            XCTAssertTrue(success)
            if !success {
                return -1
            }
        }

        let process = Process.launchedProcess(launchPath: command, arguments: arguments)
        process.waitUntilExit()
        
        if let _ = workingDirectory {
            success = fileManager.changeCurrentDirectoryPath(prevDirectory)
            XCTAssertTrue(success)
            if !success {
                return -1
            }
        }
        
        return process.terminationStatus
    }
    
    func createFixedData(size: Int = 1234) -> Data {
        var data = Data()
        var byte = getRandom()
        for _ in 0 ..< size {
            data.append(&byte, count: 1)
        }
        return data
    }
    
    func createRandomData(size: Int = 1234) -> Data {
        var data = Data()
        for _ in 0 ..< size {
            var byte = getRandom()
            data.append(&byte, count: 1)
        }
        return data
    }
    
    func createFiles(files: [String : Data], baseDirectory: String) -> Bool {
        var success = true
        for fileName in files.keys {
            if fileName.hasSuffix("/") {
                let directory = baseDirectory + fileName
                var isDirectory = ObjCBool(false)
                if !fileManager.fileExists(atPath: directory, isDirectory: &isDirectory) {
                    do {
                        try fileManager.createDirectory(atPath: directory, withIntermediateDirectories: true, attributes: nil)
                    }
                    catch {
                        success = false
                        break
                    }
                }
                else {
                    #if os(Linux)
                        let _isDirectory: Bool = isDirectory
                    #else
                        let _isDirectory: Bool = isDirectory.boolValue
                    #endif
                    if !_isDirectory {
                        success = false
                        break
                    }
                }
            }
            else {
                let data = files[fileName]!
                let stragePath = baseDirectory + fileName
                let directory = URL(fileURLWithPath: stragePath).deletingLastPathComponent().path
                if !fileManager.fileExists(atPath: directory) {
                    do {
                        try fileManager.createDirectory(atPath: directory, withIntermediateDirectories: true, attributes: nil)
                    }
                    catch {
                        success = false
                        break
                    }
                }
                
                do {
                    try data.write(to: URL(fileURLWithPath: stragePath), options: .atomic)
                }
                catch {
                    success = false
                }
                
                if !success {
                    break
                }
            }
        }
        return success
    }
    
    private func getRandom() -> UInt8 {
        #if os(Linux)
            return UInt8(rand() % 0x0100)
        #else
            return UInt8(arc4random() % 0x0100)
        #endif
    }
    
}
