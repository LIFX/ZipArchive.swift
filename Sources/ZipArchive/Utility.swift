//
//  Utility.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2017/07/04.
//  Copyright © 2017 yaslab. All rights reserved.
//

import Foundation
import CoreZipArchive

public let DefaultBufferSize = CZDefaultBufferSize

#if os(Linux)
extension ObjCBool {
    
    var boolValue: Bool {
        return self
    }
    
}
#endif

public func getcrc32(path: String) -> UInt32 {
    guard let stream = InputStream(fileAtPath: path) else {
        return 0
    }
    if stream.streamStatus == .notOpen {
        stream.open()
    }
    var result: UInt = 0
    var buffer = [UInt8](repeating: 0, count: DefaultBufferSize)
    buffer.withUnsafeMutableBufferPointer { (pointer) -> Void in
        while true {
            let len = stream.read(pointer.baseAddress!, maxLength: pointer.count)
            if len <= 0 {
                break
            }
            result = crc32(result, pointer.baseAddress!, UInt32(len))
        }
    }
    stream.close()
    return UInt32(result)
}

public func getcrc32(data: Data) -> UInt32 {
    let count = UInt32(data.count)
    return data.withUnsafeBytes { (bytes: UnsafePointer<UInt8>) in
        return UInt32(crc32(0, bytes, count))
    }
}
