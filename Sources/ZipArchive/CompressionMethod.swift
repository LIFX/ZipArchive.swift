//
//  CompressionMethod.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2017/06/27.
//  Copyright © 2016 yaslab. All rights reserved.
//

import CoreZipArchive

public struct CompressionMethod: RawRepresentable {
    
    public let rawValue: Int
    
    public init(rawValue: Int) {
        self.rawValue = rawValue
    }
    
    public static let store     = CompressionMethod(rawValue: 0)
    public static let deflate   = CompressionMethod(rawValue: Int(Z_DEFLATED))
    
}
