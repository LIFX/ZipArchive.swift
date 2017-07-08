//
//  ZipStreamTests.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2017/07/08.
//  Copyright © 2017 yaslab. All rights reserved.
//

import Foundation
import XCTest
import ZipArchive

class ZipStreamTests: XCTestCase {
    
    static let allTests = [
        ("testMemoryReadWrite", testMemoryReadWrite),
        ("testDataReadWrite", testDataReadWrite),
        ("testFileReadWrite", testFileReadWrite),
        ("testMemorySeekTell", testMemorySeekTell),
        ("testDataSeekTell", testDataSeekTell),
        ("testFileSeekTell", testFileSeekTell)
    ]
    
    override func setUp() {
        super.setUp()
        // Put setup code here. This method is called before the invocation of each test method in the class.
    }
    
    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        super.tearDown()
    }
    
    // MARK: - Read / Write
    
    func testMemoryReadWrite() {
        let stream = ZipStream(toMemory: ())
        
        // do test
        _testReadWrite(stream: stream)
        
        _ = stream.close()
    }

    func testDataReadWrite() {
        let stream = ZipStream(data: Data(repeating: 0, count: 128))
        
        // do test
        _testReadWrite(stream: stream)
        
        _ = stream.close()
    }

    func testFileReadWrite() {
        let tempFile = URL(fileURLWithPath: NSTemporaryDirectory(), isDirectory: true)
            .appendingPathComponent(UUID().uuidString)
        guard let stream = ZipStream(fileAtPath: tempFile.path, mode: .write) else {
            XCTFail()
            return
        }
        defer { try? FileManager.default.removeItem(at: tempFile) }
        
        // do test
        _testReadWrite(stream: stream)
        
        _ = stream.close()
    }
    
    private func _testReadWrite(stream: ZipStream) {
        let writeBuffer: [UInt8] = [1, 2, 3, 4, 10, 20, 30, 40]
        var readBuffer = [UInt8](repeating: 0, count: writeBuffer.count)

        let writeLen = stream.write(writeBuffer, count: writeBuffer.count)
        XCTAssertEqual(writeLen, writeBuffer.count)
        
        _ = stream.seek(offset: 0, origin: .begin)
        
        let readLen = stream.read(&readBuffer, count: readBuffer.count)
        XCTAssertEqual(readLen, readBuffer.count)
        
        XCTAssertEqual(writeBuffer, readBuffer)
    }
    
    // MARK: - Seek / Tell
    
    func testMemorySeekTell() {
        var buffer: [UInt8] = [1, 2, 3, 4]
        
        let stream = ZipStream(toMemory: ())
        XCTAssertEqual(stream.tell(), 0)

        let len = stream.write(&buffer, count: buffer.count)
        XCTAssertEqual(len, buffer.count)
        XCTAssertEqual(stream.tell(), Int64(buffer.count))

        // do test
        _testSeekTell(stream: stream, size: buffer.count)
        
        _ = stream.close()
    }

    func testDataSeekTell() {
        let buffer: [UInt8] = [1, 2, 3, 4]
        
        let stream = ZipStream(data: Data(buffer))
        XCTAssertEqual(stream.tell(), 0)

        // do test
        _testSeekTell(stream: stream, size: buffer.count)
        
        _ = stream.close()
    }

    func testFileSeekTell() {
        var buffer: [UInt8] = [1, 2, 3, 4]

        let tempFile = URL(fileURLWithPath: NSTemporaryDirectory(), isDirectory: true)
            .appendingPathComponent(UUID().uuidString)
        guard let s = OutputStream(url: tempFile, append: false) else {
            XCTFail()
            return
        }
        s.open()
        s.write(buffer, maxLength: buffer.count)
        s.close()
        defer { try? FileManager.default.removeItem(at: tempFile) }
        
        guard let stream = ZipStream(fileAtPath: tempFile.path, mode: .read) else {
            XCTFail()
            return
        }
        XCTAssertEqual(stream.tell(), 0)

        // do test
        _testSeekTell(stream: stream, size: buffer.count)
        
        _ = stream.close()
    }
    
    private func _testSeekTell(stream: ZipStream, size: Int) {
        _ = stream.seek(offset: 0, origin: .begin)
        XCTAssertEqual(stream.tell(), 0)
        _ = stream.seek(offset: 1, origin: .begin)
        XCTAssertEqual(stream.tell(), 1)
        _ = stream.seek(offset: 2, origin: .begin)
        XCTAssertEqual(stream.tell(), 2)
        
        _ = stream.seek(offset: 0, origin: .current)
        XCTAssertEqual(stream.tell(), 2)
        _ = stream.seek(offset: 1, origin: .current)
        XCTAssertEqual(stream.tell(), 3)
        _ = stream.seek(offset: 1, origin: .current)
        XCTAssertEqual(stream.tell(), 4)
        _ = stream.seek(offset: -2, origin: .current)
        XCTAssertEqual(stream.tell(), 2)
        _ = stream.seek(offset: -2, origin: .current)
        XCTAssertEqual(stream.tell(), 0)
        
        _ = stream.seek(offset: 0, origin: .end)
        XCTAssertEqual(stream.tell(), Int64(size))
        _ = stream.seek(offset: -1, origin: .end)
        XCTAssertEqual(stream.tell(), Int64(size - 1))
        _ = stream.seek(offset: -2, origin: .end)
        XCTAssertEqual(stream.tell(), Int64(size - 2))
    }
    
}
