//
//  CompressionLevel.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2017/06/27.
//  Copyright © 2016 yaslab. All rights reserved.
//

import CoreZipArchive

public struct CompressionLevel: RawRepresentable {
    
    public let rawValue: Int
    
    public init(rawValue: Int) {
        self.rawValue = rawValue
    }
    
    public static let noCompression = CompressionLevel(rawValue: Int(Z_NO_COMPRESSION))
    public static let fastest       = CompressionLevel(rawValue: Int(Z_BEST_SPEED))
    public static let optimal       = CompressionLevel(rawValue: Int(Z_BEST_COMPRESSION))
    public static let `default`     = CompressionLevel(rawValue: Int(Z_DEFAULT_COMPRESSION))
    
}
