//
//  Unzip+ZipFile.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2015/11/07.
//  Copyright © 2015 yaslab. All rights reserved.
//

import Foundation

extension Unzip {
    
    private func createBaseDirectory(root: String, subpath: String) throws {
        var path = subpath
        var baseURL = URL(fileURLWithPath: root)
        
        if let range = path.range(of: "/", options: .backwards) {
            path = path.substring(to: range.lowerBound)
            baseURL = baseURL.appendingPathComponent(path)
        }
        
        let fm = FileManager.default
        if !fm.fileExists(atPath: baseURL.path) {
            try fm.createDirectory(atPath: baseURL.path, withIntermediateDirectories: true, attributes: nil)
        }
    }
    
    private func extractDirectory(_ entry: Unzip.Entry, root: String) throws {
        let fullURL = URL(fileURLWithPath: root).appendingPathComponent(entry.name)
        
        let fm = FileManager.default
        if !fm.fileExists(atPath: fullURL.path) {
            try fm.createDirectory(atPath: fullURL.path, withIntermediateDirectories: true, attributes: nil)
        }
    }
    
    private func extractFile(_ entry: Unzip.Entry, root: String) throws {
        let fullURL = URL(fileURLWithPath: root).appendingPathComponent(entry.name)
        try createBaseDirectory(root: root, subpath: entry.name)
        try entry.extract(toFileAtPath: fullURL.path, overwrite: false)
    }
    
    //private func extractSymlink(_ entry: Unzip.Entry, root: String) throws {
        //let fullURL = URL(fileURLWithPath: root).appendingPathComponent(entry.fileName)
        
        //        let fullPath = directory.appendingPathComponent(entry.fullName)
        //        let data = try entry.extractToData()
        //        guard let destination = String(data: data, encoding: .utf8) else {
        //            throw ZipError.io
        //        }
        //
        //        try createBaseDirectory(entry)
        //
        //        try fm.createSymbolicLink(atPath: fullPath, withDestinationPath: destination)
    //}
    
    public func extract(toDirectoryAtPath path: String, password: String? = nil) throws {
        try self.enumerateEntries { (entry, _) in
            entry.open(password: password, decompressFactory: nil)
            if entry.isDirectory {
                try extractDirectory(entry, root: path)
            } else {
                try extractFile(entry, root: path)
            }
            entry.close()
        }
    }
    
}

extension Unzip.Entry {

    public func extract(toFileAtPath path: String, overwrite: Bool = false) throws {
        let fileManager = FileManager.default
        
        if fileManager.fileExists(atPath: path) {
            if !overwrite {
                throw ZipError.io
            }
        }

        guard let stream = OutputStream(toFileAtPath: path, append: false) else {
            throw ZipError.io
        }
        stream.open()
        defer {
            stream.close()
        }
        
        var _buffer = [UInt8](repeating: 0, count: DefaultBufferSize)
        let len = try _buffer.withUnsafeMutableBufferPointer { (buffer) -> Int in
            var totalLen = 0
            
            while true {
                let readLen = self.read(buffer.baseAddress!, count: buffer.count)
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
            
            return totalLen
        }
        
        if len != self.size {
            // FIXME:
            print("totalLen != self.originalSize")
            throw ZipError.invalidData
        }
    }
    
}
