//
//  Unzip+ZipFile.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2015/11/07.
//  Copyright © 2015 yaslab. All rights reserved.
//

import Foundation

extension Unzip {

    public func extract(toDirectoryAtPath path: String, password: String? = nil) throws {
        let dirURL = URL(fileURLWithPath: path, isDirectory: true)
        try self.enumerateEntries { (entry, _) in
            let url = dirURL.appendingPathComponent(entry.name, isDirectory: entry.isDirectory)
            try entry.extract(toFileAtPath: url.path, overwrite: false, password: password)
        }
    }
    
}

extension Unzip.Entry {
    
    public func extractToData(password: String? = nil) throws -> Data {
        var data = Data()
        var _buffer = [UInt8](repeating: 0, count: DefaultBufferSize)
        try _buffer.withUnsafeMutableBufferPointer { (buffer) -> Void in
            try self.extract(password: password) { (fileReader) in
                while true {
                    let readLen = fileReader.read(buffer.baseAddress!, count: buffer.count)
                    if readLen < 0 {
                        throw ZipError.io
                    }
                    if readLen == 0 {
                        break
                    }
                    data.append(buffer.baseAddress!, count: readLen)
                }
            }
        }
        return data
    }
    
    public func extract(toFileAtPath path: String, overwrite: Bool = false, password: String? = nil) throws {
        let url = URL(fileURLWithPath: path, isDirectory: self.isDirectory)
        let fileManager = FileManager.default
        
        if self.isDirectory {
            if !fileManager.fileExists(atPath: url.path) {
                try fileManager.createDirectory(at: url, withIntermediateDirectories: true, attributes: nil)
            }
            return
        } else {
            if fileManager.fileExists(atPath: url.path) {
                if !overwrite {
                    throw ZipError.io
                }
            }
            
            let dirURL = url.deletingLastPathComponent()
            if !fileManager.fileExists(atPath: dirURL.path) {
                try fileManager.createDirectory(at: dirURL, withIntermediateDirectories: true, attributes: nil)
            }
        }

        guard let stream = OutputStream(url: url, append: false) else {
            throw ZipError.io
        }
        stream.open()
        defer {
            stream.close()
        }
        
        var _buffer = [UInt8](repeating: 0, count: DefaultBufferSize)
        let len = try _buffer.withUnsafeMutableBufferPointer { (buffer) -> Int in
            var totalLen = 0
            
            try self.extract(password: password) { (fileReader) in
                while true {
                    let readLen = fileReader.read(buffer.baseAddress!, count: buffer.count)
                    if readLen < 0 {
                        throw ZipError.io
                    }
                    if readLen == 0 {
                        // END
                        break
                    }
                    let err = stream.write(buffer.baseAddress!, maxLength: readLen)
                    if err < 0 {
                        throw ZipError.io
                    }
                    totalLen += readLen
                }
            }
            
            return totalLen
        }
        
        if len != self.size {
            // FIXME:
            print("totalLen != self.originalSize")
            throw ZipError.invalidData
        }
    }

}
