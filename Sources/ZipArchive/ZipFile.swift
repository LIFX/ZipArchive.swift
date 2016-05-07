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
        sourceDirectoryName: String,
        destinationArchiveFileName: String,
        compressionLevel: CompressionLevel = .default,
        includeBaseDirectory: Bool = false,
        entryNameEncoding: String.Encoding = .utf8)
        throws
    {
        try createFromDirectory(
            sourceDirectoryName: sourceDirectoryName,
            destinationArchiveFileName: destinationArchiveFileName,
            compressionLevel: compressionLevel,
            includeBaseDirectory: includeBaseDirectory,
            entryNameEncoding: entryNameEncoding,
            encryptionStreamFactory: nil)
    }
    
    public class func createFromDirectory(
        sourceDirectoryName: String,
        destinationArchiveFileName: String,
        encryptionStreamFactory: ZipEncryptionStreamFactory,
        compressionLevel: CompressionLevel = .default,
        includeBaseDirectory: Bool = false,
        entryNameEncoding: String.Encoding = .utf8)
        throws
    {
        try createFromDirectory(
            sourceDirectoryName: sourceDirectoryName,
            destinationArchiveFileName: destinationArchiveFileName,
            compressionLevel: compressionLevel,
            includeBaseDirectory: includeBaseDirectory,
            entryNameEncoding: entryNameEncoding,
            encryptionStreamFactory: encryptionStreamFactory)
    }
    
    private class func createFromDirectory(
        sourceDirectoryName: String,
        destinationArchiveFileName: String,
        compressionLevel: CompressionLevel,
        includeBaseDirectory: Bool,
        entryNameEncoding: String.Encoding,
        encryptionStreamFactory: ZipEncryptionStreamFactory?)
        throws
    {
        let fm = FileManager.default
        let subpaths = try fm.subpathsOfDirectory(atPath: sourceDirectoryName)
        
        let zip = try ZipArchive(path: destinationArchiveFileName, mode: .create, entryNameEncoding: entryNameEncoding)
        defer {
            zip.dispose()
        }
        
        let directoryName = sourceDirectoryName as NSString
        let baseDirectoryName = directoryName.lastPathComponent as NSString
        
        for subpath in subpaths {
            let fullpath = directoryName.appendingPathComponent(subpath)
            var entryName = subpath
            if includeBaseDirectory {
                entryName = baseDirectoryName.appendingPathComponent(entryName)
            }
            let entry = try zip.createEntryFrom(filePath: fullpath, entryName: entryName, compressionLevel: compressionLevel, encryptionStreamFactory: encryptionStreamFactory)
        }
    }
    
    public class func extractToDirectory(sourceArchiveFileName: String, destinationDirectoryName: String, entryNameEncoding: String.Encoding = .utf8) throws {
        try extractToDirectory(sourceArchiveFileName: sourceArchiveFileName, destinationDirectoryName: destinationDirectoryName, entryNameEncoding: entryNameEncoding, encryptionStreamFactory: nil)
    }

    public class func extractToDirectory(sourceArchiveFileName: String, destinationDirectoryName: String, encryptionStreamFactory: ZipEncryptionStreamFactory, entryNameEncoding: String.Encoding = .utf8) throws {
        try extractToDirectory(sourceArchiveFileName: sourceArchiveFileName, destinationDirectoryName: destinationDirectoryName, entryNameEncoding: entryNameEncoding, encryptionStreamFactory: encryptionStreamFactory)
    }
    
    private class func extractToDirectory(sourceArchiveFileName: String, destinationDirectoryName: String, entryNameEncoding: String.Encoding, encryptionStreamFactory: ZipEncryptionStreamFactory?) throws {
        let unzip = try ZipArchive(path: sourceArchiveFileName, mode: .read, entryNameEncoding: entryNameEncoding)
        defer {
            unzip.dispose()
        }
        
        try unzip.extractTo(directoryPath: destinationDirectoryName, encryptionStreamFactory: encryptionStreamFactory)
    }
    
    public class func openRead(archiveFileName: String) throws -> ZipArchive {
        return try open(archiveFileName: archiveFileName, mode: .read, entryNameEncoding: .utf8)
    }
    
    public class func open(archiveFileName: String, mode: ZipArchiveMode) throws -> ZipArchive {
        return try open(archiveFileName: archiveFileName, mode: mode, entryNameEncoding: .utf8)
    }
    
    public class func open(archiveFileName: String, mode: ZipArchiveMode, entryNameEncoding: String.Encoding, passwordEncoding: String.Encoding = .ascii) throws -> ZipArchive {
        return try ZipArchive(path: archiveFileName, mode: mode, entryNameEncoding: entryNameEncoding)
    }
    
}