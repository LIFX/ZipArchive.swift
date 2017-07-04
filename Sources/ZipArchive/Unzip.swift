//
//  Unzip.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/22.
//  Copyright © 2016 yaslab. All rights reserved.
//

import Foundation
import CoreZipArchive

public class Unzip {
    
    public class Entry {
        
        public typealias DecompressFactory = CZDecompressFactoryFunc
        
        fileprivate let ptr: CZUnzipEntryRef
        fileprivate let encoding: String.Encoding
        
        fileprivate let globalHeader: EntryHeader
        fileprivate var localHeader: LocalEntryHeader?
        
        public var name: String {
            return globalHeader.entryName
        }
        public var size: Int {
            return globalHeader.size
        }
        public var compressedSize: Int {
            return globalHeader.compressedSize
        }
        public var lastWriteTime: ZipArchiveDateTime {
            return globalHeader.dateTime
        }
        
        public var isDirectory: Bool {
            return globalHeader.entryName.hasSuffix("/")
        }
        public var isCompressed: Bool {
            return globalHeader.method != .store
        }
        public var isEncrypted: Bool {
            return (globalHeader.flags & 1) != 0
        }
        
        init(_ ptr: CZUnzipEntryRef, globalHeader: CZEntryHeaderRef, encoding: String.Encoding) {
            self.ptr = ptr
            self.globalHeader = EntryHeader(globalHeader, encoding: encoding)
            self.encoding = encoding
        }
        
        deinit {
            CZUnzipEntryRelease(ptr)
        }
        
        public func open(password: String? = nil, decompressFactory: DecompressFactory? = nil) {
            if let password = password {
                guard let cstrPass = password.cString(using: .ascii) else {
                    // TODO:
                    fatalError()
                }
                guard cstrPass.count > 0 else {
                    // TODO:
                    //throw ...
                    fatalError()
                }
                CZUnzipEntryOpen(ptr, cstrPass, decompressFactory)
            } else {
                CZUnzipEntryOpen(ptr, nil, decompressFactory)
            }
            
            localHeader = EntryHeader(CZUnzipEntryGetLocalHeader(ptr), encoding: encoding)
        }
        
        public func read(_ buffer: UnsafeMutablePointer<UInt8>, count: Int) -> Int {
            return CZUnzipEntryRead(ptr, buffer, count)
        }
        
        public func validateChecksum() -> Bool {
            let expectedValue = globalHeader.crc
            let actualValue = CZUnzipGetCRC32(ptr)
            // FIXME: Swift 4 ではキャスト不要
            return expectedValue == UInt(actualValue)
        }
        
        public func close() {
            CZUnzipEntryClose(ptr)
        }
        
    }

    fileprivate let ptr: CZUnzipRef
    fileprivate let stream: ArchiveStream
    fileprivate let encoding: String.Encoding
    
    public convenience init?(data: Data, entryNameEncoding encoding: String.Encoding = .utf8) {
        self.init(stream: ArchiveStream(data: data), entryNameEncoding: encoding)
    }
    
    public convenience init?(fileAtPath path: String, entryNameEncoding encoding: String.Encoding = .utf8) {
        guard let stream = ArchiveStream(fileAtPath: path, mode: .read) else {
            return nil
        }
        self.init(stream: stream, entryNameEncoding: encoding)
    }
    
    public init?(stream: ArchiveStream, entryNameEncoding encoding: String.Encoding = .utf8) {
        guard let ptr = CZUnzipCreate(stream.ptr) else {
            return nil
        }
        self.ptr = ptr
        self.stream = stream
        self.encoding = encoding
    }
    
    deinit {
        CZUnzipRelease(ptr)
    }

}

extension Unzip: IteratorProtocol, Sequence {
    
    public func next() -> Entry? {
        guard CZUnzipMoveNextEntry(ptr) else {
            return nil
        }
        let entry = CZUnzipGetCurrentEntry(ptr)!
        let globalHeader = CZUnzipGetCurrentGlobalHeader(ptr)!
        return Entry(entry, globalHeader: globalHeader, encoding: encoding)
    }

    public func resetIterator() {
        CZUnzipResetIterator(ptr)
    }
    
    public func enumerateEntries(invoking: (Entry, inout Bool) throws -> Void) rethrows {
        CZUnzipResetIterator(ptr)
        var stop = false
        while let entry = next() {
            try invoking(entry, &stop)
            if stop {
                break
            }
        }
    }
    
}
