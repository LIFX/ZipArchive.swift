//
//  ZipStreamTests.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2017/07/08.
//  Copyright © 2017 yaslab. All rights reserved.
//

import XCTest
import ZipArchive

class ZipStreamTests: XCTestCase {
    
    override func setUp() {
        super.setUp()
        // Put setup code here. This method is called before the invocation of each test method in the class.
    }
    
    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        super.tearDown()
    }
    
    func testMemorySeek() {
        var buffer: [UInt8] = [1, 2, 3, 4]
        
        let stream = ZipStream(toMemory: ())
        XCTAssertEqual(stream.tell(), 0)

        let len = stream.write(&buffer, count: buffer.count)
        XCTAssertEqual(len, buffer.count)
        XCTAssertEqual(stream.tell(), Int64(buffer.count))

        // do test
        _testSeek(stream: stream, size: buffer.count)
        
        _ = stream.close()
    }

    func testDataSeek() {
        let buffer: [UInt8] = [1, 2, 3, 4]
        
        let stream = ZipStream(data: Data(buffer))
        XCTAssertEqual(stream.tell(), 0)

        // do test
        _testSeek(stream: stream, size: buffer.count)
        
        _ = stream.close()
    }

    func testFileSeek() {
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
        _testSeek(stream: stream, size: buffer.count)
        
        _ = stream.close()
    }
    
    private func _testSeek(stream: ZipStream, size: Int) {
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
