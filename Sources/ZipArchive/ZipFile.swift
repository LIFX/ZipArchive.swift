//
//  ZipFile.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2015/11/07.
//  Copyright © 2015 yaslab. All rights reserved.
//

import Foundation

public class ZipFile {
    
    private init() {}

    public class func createFromDirectory(
        at directoryPath: String,
        to archiveFilePath: String,
        compressionLevel: CompressionLevel = .default,
        includeBaseDirectory: Bool = false,
        entryNameEncoding: String.Encoding = .utf8,
        password: String? = nil)
        throws
    {

        let fm = FileManager.default
        let subpaths = try fm.subpathsOfDirectory(atPath: directoryPath)

        //let zip = try ZipArchive(path: destinationArchiveFileName, mode: .create, entryNameEncoding: entryNameEncoding)
        guard let zip = Zip(toFileAtPath: archiveFilePath, entryNameEncoding: entryNameEncoding) else {
            throw ZipError.io
        }
        defer {
            //zip.dispose()
            zip.close()
        }

        let directoryName = NSString(string: directoryPath)
        let baseDirectoryName = NSString(string: directoryName.lastPathComponent)
        
        for subpath in subpaths {
            let fullpath = directoryName.appendingPathComponent(subpath)
            var entryName = subpath
            if includeBaseDirectory {
                entryName = baseDirectoryName.appendingPathComponent(entryName)
            }

            try zip.appendEntry(fromFileAtPath: fullpath, entryName: entryName, method: .deflate, level: compressionLevel, password: password)
        }
    }
    
    public class func extractToDirectory(
        at directoryPath: String,
        from archiveFilePath: String,
        entryNameEncoding: String.Encoding = .utf8,
        password: String? = nil)
        throws
    {
        guard let unzip = Unzip(fileAtPath: archiveFilePath, entryNameEncoding: entryNameEncoding) else {
            throw ZipError.io
        }
        defer {
            //unzip.dispose()
        }
        
        try unzip.extract(toDirectoryAtPath: directoryPath, password: password)
    }
    
}
