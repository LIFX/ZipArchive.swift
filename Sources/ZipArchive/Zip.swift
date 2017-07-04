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
        
        init(_ ptr: CZZipEntryRef) {
            self.ptr = ptr
        }
        
        deinit {
            CZZipEntryRelease(ptr)
        }
        
        public func write(_ buffer: UnsafePointer<UInt8>, count: Int) -> Int {
            return CZZipEntryWrite(ptr, buffer, count)
        }
        
        public func close() {
            var dummy: UInt8 = 0
            let ret = write(&dummy, count: 0) // flush
            if ret != 0 {
                // ERROR
            }
            CZZipEntryClose(ptr)
        }
        
    }
    
    fileprivate let ptr: CZZipRef
    fileprivate let stream: ArchiveStream
    fileprivate let encoding: String.Encoding

    public convenience init(toMemory: (), entryNameEncoding encoding: String.Encoding = .utf8) {
        self.init(stream: ArchiveStream(toMemory: ()), entryNameEncoding: encoding)
    }
    
    public convenience init?(toFileAtPath path: String, entryNameEncoding encoding: String.Encoding = .utf8) {
        guard let stream = ArchiveStream(fileAtPath: path, mode: .write) else {
            return nil
        }
        self.init(stream: stream, entryNameEncoding: encoding)
    }

    public init(stream: ArchiveStream, entryNameEncoding encoding: String.Encoding = .utf8) {
        self.ptr = CZZipCreate(stream.ptr)
        self.stream = stream
        self.encoding = encoding
    }
    
    deinit {
        CZZipRelease(ptr)
    }
    
    public func appendEntry(_ entryName: String, method: CompressionMethod = .deflate, level: CompressionLevel = .optimal, password: String? = nil, crc32: UInt32 = 0) -> Entry {
        // 引数: 日付、パーミッション、ディレクトリかどうか
        // TODO: ディレクトリの場合の考慮
        
        // FIXME:
        let time = ZipArchiveDateTime(year: 2017, month: 1, day: 2, hour: 3, minute: 4, second: 5)
        
        guard let cstrName = entryName.cString(using: encoding) else {
            // FIXME: 
            fatalError()
        }

        var cstrPass: [CChar]? = nil
        if let password = password {
            guard let cstr = password.cString(using: .ascii) else {
                // FIXME:
                fatalError()
            }
            guard cstr.count > 0 else {
                // FIXME:
                //throw ...
                fatalError()
            }
            cstrPass = cstr
        }

        return Entry(CZZipAppendEntry(ptr, cstrName, method.rawValue, time, level.rawValue, nil, cstrPass, crc32))
    }
    
    public func close() {
        CZZipClose(ptr)
    }
    
}

extension Zip {
    
    public var memoryData: Data? {
        return stream.memoryData
    }
    
}
