//
//  Zip.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2017/06/10.
//  Copyright © 2016 yaslab. All rights reserved.
//

import Foundation
import CoreZipArchive

public class Zip {
    
    public class Entry {
        
        fileprivate let ptr: CZZipEntryRef

        fileprivate init(_ ptr: CZZipEntryRef) {
            self.ptr = ptr
        }
        
        deinit {
            CZZipEntryRelease(ptr)
        }

        public func write(_ buffer: UnsafePointer<UInt8>, count: Int) -> Int {
            return CZZipEntryWrite(ptr, buffer, count)
        }
        
        fileprivate func close() throws {
            // flush
            var dummy: UInt8 = 0
            if write(&dummy, count: 0) != 0 {
                throw ZipError.io
            }
            
            if !CZZipEntryClose(ptr) {
                throw ZipError.io
            }
        }
        
    }
    
    fileprivate let ptr: CZZipRef
    fileprivate let stream: ZipStream
    fileprivate let encoding: String.Encoding

    public convenience init(toMemory: (), entryNameEncoding encoding: String.Encoding = .utf8) {
        self.init(stream: ZipStream(toMemory: ()), entryNameEncoding: encoding)
    }
    
    public convenience init?(toFileAtPath path: String, entryNameEncoding encoding: String.Encoding = .utf8) {
        guard let stream = ZipStream(fileAtPath: path, mode: .write) else {
            return nil
        }
        self.init(stream: stream, entryNameEncoding: encoding)
    }

    public init(stream: ZipStream, entryNameEncoding encoding: String.Encoding = .utf8) {
        self.ptr = CZZipCreate(stream.ptr)
        self.stream = stream
        self.encoding = encoding
    }
    
    deinit {
        CZZipRelease(ptr)
    }
    
    public func appendEntry(_ entryName: String, isDirectory: Bool = false, method: CompressionMethod = .deflate, level: CompressionLevel = .optimal, password: String? = nil, crc32: UInt32 = 0, writeBlock: ((Entry) throws -> Void)) throws {
        // 引数: 日付、パーミッション
        
        var entryName = entryName
        var method = method
        var level = level
        var password = password
        
        if isDirectory {
            if !entryName.hasSuffix("/") {
                entryName += "/"
            }
            method = .store
            level = .noCompression
            password = nil
        }
        
        // FIXME:
        let time = ZipArchiveDateTime(year: 2017, month: 1, day: 2, hour: 3, minute: 4, second: 5)
        
        guard let cstrName = entryName.cString(using: encoding) else {
            throw ZipError.stringEncodingMismatch
        }

        var cstrPass: [CChar]? = nil
        if let password = password {
            guard let cstr = password.cString(using: .ascii) else {
                throw ZipError.stringEncodingMismatch
            }
            guard cstr.count > 0 else {
                throw ZipError.invalidData
            }
            cstrPass = cstr
        }

        let entry = Entry(CZZipAppendEntry(ptr, cstrName, method.rawValue, time, level.rawValue, nil, cstrPass, crc32))
        var e: Error? = nil
        do {
            try writeBlock(entry)
        } catch {
            e = error
        }
        do {
            try entry.close()
        } catch {
            if e == nil {
                e = error
            }
        }
        if let error = e {
            throw error
        }
    }
    
    public func close() {
        CZZipClose(ptr)
        _ = stream.close()
    }
    
}

extension Zip {
    
    public var memoryData: Data? {
        return stream.memoryData
    }
    
}
