//
//  LocalEntryHeader.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/22.
//  Copyright © 2016 yaslab. All rights reserved.
//

import CoreZipArchive

public protocol LocalEntryHeader {
    
    /// version needed to extract
    var versionNeededToExtract: Int { get set }
    /// general purpose bit flag
    var flags: UInt { get set }
    /// compression method
    var method: CompressionMethod { get set }
    /// last mod file date & time
    var dateTime: ZipArchiveDateTime { get set }
    /// crc-32
    var crc: UInt { get set }
    /// compressed size
    var compressedSize: Int { get set }
    /// uncompressed size
    var size: Int { get set }
    
    /// entry name
    var entryName: String { get set }
    
}

extension EntryHeader: LocalEntryHeader {

}
