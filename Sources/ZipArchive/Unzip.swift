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
        
        private func open(password: String? = nil, decompressFactory: DecompressFactory? = nil) {
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

        private func close() {
            CZUnzipEntryClose(ptr)
        }
        
        public func extract(password: String? = nil, decompressFactory: DecompressFactory? = nil, readBlock: ((EntryReader) throws -> Void)) rethrows {
            var password = password
            if self.isDirectory {
                password = nil
            }
            self.open(password: password, decompressFactory: decompressFactory)
            defer { self.close() }
            try readBlock(EntryReader(ptr, crc32: globalHeader.crc))
        }
        
    }
    
    public class EntryReader {

        fileprivate let ptr: CZUnzipEntryRef // weak ref
        fileprivate let crc32: UInt
        
        fileprivate init(_ ptr: CZUnzipEntryRef, crc32: UInt) {
            self.ptr = ptr
            self.crc32 = crc32
        }
        
        public func read(_ buffer: UnsafeMutablePointer<UInt8>, count: Int) -> Int {
            return CZUnzipEntryRead(ptr, buffer, count)
        }
        
        public func validateChecksum() -> Bool {
            let expectedValue = crc32
            let actualValue = CZUnzipGetCRC32(ptr)
            // FIXME: Swift 4 ではキャスト不要
            return expectedValue == UInt(actualValue)
        }
        
    }

    fileprivate let ptr: CZUnzipRef
    fileprivate let stream: ZipStream
    fileprivate let encoding: String.Encoding
    
    public convenience init?(data: Data, entryNameEncoding encoding: String.Encoding = .utf8) {
        self.init(stream: ZipStream(data: data), entryNameEncoding: encoding)
    }
    
    public convenience init?(fileAtPath path: String, entryNameEncoding encoding: String.Encoding = .utf8) {
        guard let stream = ZipStream(fileAtPath: path, mode: .read) else {
            return nil
        }
        self.init(stream: stream, entryNameEncoding: encoding)
    }
    
    public init?(stream: ZipStream, entryNameEncoding encoding: String.Encoding = .utf8) {
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

    public func close() {
        _ = stream.close()
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
    
    public func enumerateEntries(invoking block: (Entry, inout Bool) throws -> Void) rethrows {
        CZUnzipResetIterator(ptr)
        var stop = false
        while let entry = next() {
            try block(entry, &stop)
            if stop {
                break
            }
        }
    }
    
}
