//
//  ZipConstants.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2015/11/08.
//  Copyright © 2015 yaslab. All rights reserved.
//

import Foundation
import CoreZipArchive

public let DefaultBufferSize = CZDefaultBufferSize

public enum ZipError: Error {
    
    case argument
    case argumentNull
    case pathTooLong
    case directoryNotFound
    case io
    case unauthorizedAccess
    case fileNotFound
    case invalidData
    case objectDisposed
    case notSupported
    
    case stringEncodingMismatch
    
}
