//
//  EntryHeader.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/22.
//  Copyright © 2016 yaslab. All rights reserved.
//

import CoreZipArchive

public struct EntryHeader {

    /// version made by (lower byte)
    public var versionMadeBy: Int
    /// version made by (upper byte)
    public var systemInfo: Int
    
    /// version needed to extract
    public var versionNeededToExtract: Int
    /// general purpose bit flag
    public var flags: UInt
    /// compression method
    public var method: CompressionMethod
    /// last mod file date & time
    public var dateTime: ZipArchiveDateTime
    /// crc-32
    public var crc: UInt
    /// compressed size
    public var compressedSize: Int
    /// uncompressed size
    public var size: Int
    
    /// disk number start
    public var diskNumberStart: Int
    /// internal file attributes
    public var internalFileAttributes: UInt
    /// external file attributes
    public var externalFileAttributes: UInt
    /// relative offset of local header
    public var localHeaderOffset: Int

    /// entry name
    public var entryName: String
    
    init(_ ptr: CZEntryHeaderRef, encoding: String.Encoding) {
        let fileNameLength = CZEntryHeaderGetFileNameLength(ptr)
        var fileNameBuffer = [CChar](repeating: 0, count: fileNameLength + 1)
        CZEntryHeaderGetFileName(ptr, &fileNameBuffer)
        guard let fileName = String(cString: fileNameBuffer, encoding: encoding) else {
            // FIXME: 
            fatalError()
        }
        self.entryName = fileName
        
        let info = CZEntryHeaderGetInfo(ptr)
        self.versionNeededToExtract = Int(info.versionNeededToExtract)
        self.flags = UInt(info.flags)
        self.method = CompressionMethod(rawValue: Int(info.method))
        self.dateTime = ZipArchiveDateTime(date: Int(info.date), time: Int(info.time))
        self.crc = UInt(info.crc)
        self.compressedSize = Int(info.compressedSize)
        self.size = Int(info.size)
        self.versionMadeBy = Int(info.versionMadeBy)
        self.systemInfo = Int(info.systemInfo)
        self.diskNumberStart = Int(info.diskNumberStart)
        self.internalFileAttributes = UInt(info.internalFileAttributes)
        self.externalFileAttributes = UInt(info.externalFileAttributes)
        self.localHeaderOffset = Int(info.localHeaderOffset)
    }
    
}
