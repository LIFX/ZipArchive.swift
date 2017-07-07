//
//  Zip+ZipFile.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2016/01/26.
//  Copyright © 2016 yaslab. All rights reserved.
//

import Foundation

extension Zip {
    
    public func appendEntry(fromData data: Data, entryName: String, isDirectory: Bool = false, method: CompressionMethod = .deflate, level: CompressionLevel = .optimal, password: String? = nil) throws {
        var name = entryName
        var method = method
        var level = level
        var cstrPass: [CChar]? = nil
        var crc32: UInt32 = 0
        if isDirectory {
            if !name.hasSuffix("/") {
                name += "/"
            }
            method = .store
            level = .noCompression
        } else {
            if let password = password {
                cstrPass = password.cString(using: .ascii)
                if cstrPass == nil {
                    throw ZipError.stringEncodingMismatch
                }
            }
            crc32 = getcrc32(data: data)
        }
        
        try self.appendEntry(name, method: method, level: level, password: password, crc32: crc32) { entry in
            if isDirectory {
                // Do Nothing
            } else {
                let count = data.count
                let len = data.withUnsafeBytes { (bytes: UnsafePointer<UInt8>) in
                    return entry.write(bytes, count: count)
                }
                if len != count {
                    throw ZipError.io
                }
            }
        }
    }

    public func appendEntry(fromFileAtPath path: String, entryName: String, method: CompressionMethod = .deflate, level: CompressionLevel = .optimal, password: String? = nil) throws {
        let fileManager = FileManager.default
        
        var isDirectory: ObjCBool = false
        guard fileManager.fileExists(atPath: path, isDirectory: &isDirectory) else {
            throw ZipError.fileNotFound
        }
        
        let fileAttributes = try fileManager.attributesOfItem(atPath: path)
        guard let modificationDate = fileAttributes[.modificationDate] as? Date else {
            throw ZipError.io
        }
        
        // pre-process
        var name = entryName
        var method = method
        var level = level
        var cstrPass: [CChar]? = nil
        var crc32: UInt32 = 0
        if isDirectory.boolValue {
            if !name.hasSuffix("/") {
                name += "/"
            }
            method = .store
            level = .noCompression
        } else {
            if let password = password {
                cstrPass = password.cString(using: .ascii)
                if cstrPass == nil {
                    throw ZipError.stringEncodingMismatch
                }
            }
            crc32 = getcrc32(path: path)
        }

        try self.appendEntry(name, method: method, level: level, password: password, crc32: crc32) { entry in
            if isDirectory.boolValue {
                // Do Nothing
            } else {
                guard let fileInputStream = InputStream(fileAtPath: path) else {
                    throw ZipError.io
                }
                fileInputStream.open()
                defer {
                    fileInputStream.close()
                }
                let buffer = malloc(DefaultBufferSize).assumingMemoryBound(to: UInt8.self)
                defer {
                    free(buffer)
                }
                while true {
                    let len = fileInputStream.read(buffer, maxLength: DefaultBufferSize)
                    if len < 0 {
                        // ERROR
                        break
                    }
                    if len == 0 {
                        // END
                        break
                    }
                    let err = entry.write(buffer, count: len)
                    if err < 0 {
                        // ERROR
                        break
                    }
                }
                let err = entry.write(buffer, count: 0) // flush
                if err < 0 {
                    // ERROR
                    throw ZipError.io
                }
            }
        }
    }

}
