//
//  ZipArchiveStream.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/22.
//  Copyright © 2016 yaslab. All rights reserved.
//

import Foundation
import CoreZipArchive

extension CZStreamFileMode {
    
    public static let read: CZStreamFileMode = CZStreamFileModeRead
    public static let write: CZStreamFileMode = CZStreamFileModeWrite
    
}

extension CZStreamSeekOrigin {
    
    public static let begin: CZStreamSeekOrigin = CZStreamSeekOriginBegin
    public static let current: CZStreamSeekOrigin = CZStreamSeekOriginCurrent
    public static let end: CZStreamSeekOrigin = CZStreamSeekOriginEnd
    
}

public class ZipArchiveStream {
    
    public typealias Functions = CZStreamFunctions
    public typealias FileMode = CZStreamFileMode
    public typealias SeekOrigin = CZStreamSeekOrigin
    
    let ptr: CZStreamRef
    private var bufferData: NSMutableData?
    
    public init(toMemory: ()) {
        ptr = CZStreamCreateInMemory()
    }
    
    public init(data: Data) {
        let _data = NSMutableData(data: data)
        bufferData = _data
        ptr = CZStreamCreateWithBuffer(_data.mutableBytes, _data.length)
    }

    public init?(fileAtPath path: String, mode: FileMode) {
        guard let ptr = CZStreamCreateWithFileAtPath(path, mode) else {
            return nil
        }
        self.ptr = ptr
    }
    
    public init(functions: Functions, opaque: OpaquePointer?) {
        ptr = CZStreamCreate(functions, UnsafeMutableRawPointer(opaque))
    }
    
    deinit {
        CZStreamRelease(ptr)
    }
    
    public func read(_ buffer: UnsafeMutablePointer<UInt8>, count: Int) -> Int {
        return CZStreamRead(ptr, buffer, count)
    }
    
    public func write(_ buffer: UnsafePointer<UInt8>, count: Int) -> Int {
        return CZStreamWrite(ptr, buffer, count)
    }
    
    public func seek(offset: Int64, origin: SeekOrigin) -> Int {
        return CZStreamSeek(ptr, offset, origin)
    }
    
    public func tell() -> Int64 {
        return CZStreamTell(ptr)
    }
    
    public var errorCode: Int {
        return CZStreamGetErrorCode(ptr)
    }
    
}

extension ZipArchiveStream {
    
    public var memoryData: Data? {
        guard let buf = CZStreamGetMemoryBuffer(ptr) else {
            return nil
        }
        let len = CZStreamGetMemoryBufferLength(ptr)
        return Data(bytes: buf, count: len)
    }
    
}
