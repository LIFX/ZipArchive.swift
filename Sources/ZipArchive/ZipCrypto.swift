//
//  ZipCrypto.swift
//  ZipArchive
//
//  Created by Yasuhiro Hatta on 2017/01/11.
//  Copyright © 2017 yaslab. All rights reserved.
//

import Foundation

public enum ZipEncryptionOperation {
    
    case encrypt
    case decrypt
    
}

public protocol ZipEncryptionStream: IOStream {

    //var operation: ZipEncryptionOperation { get }
    var headerSize: Int { get }
    
}

public protocol ZipEncryptionStreamFactory {
    
    //var operation: ZipEncryptionOperation { get }
    //var isUseCRC32: Bool { get }
    
    //func create(stream: IOStream, crc32: UInt32) -> ZipEncryptionStream
    func create(stream: IOStream) -> ZipEncryptionStream

}

public class ZipCrypto {
    
    let headerSize = 12
    
    private let crc32Table: [UInt32]
    private var key0: UInt32 = 0
    private var key1: UInt32 = 0
    private var key2: UInt32 = 0
    
    init(crc32Table: [UInt32]) {
        self.crc32Table = crc32Table
    }
    
    private func crc32(_ c: UInt32, _ b: UInt32) -> UInt32 {
        let index = Int((c ^ b) & 0xff)
        return crc32Table[index] ^ (c >> 8)
    }

    private func decryptByte() -> UInt8 {
        let temp = (key2 & 0xffff) | 2
        let temp2 = ((temp &* (temp ^ 1)) >> 8) & 0xff
        return UInt8(truncatingBitPattern: temp2)
    }
    
    private func updateKeys(_ c: UInt8) {
        key0 = crc32(key0, UInt32(c))
        key1 = key1 + (key0 & 0xff)
        key1 = (key1 &* 134775813) &+ 1
        key2 = crc32(key2, key1 >> 24)
    }
    
    fileprivate func initKeys(password: [CChar]) {
        key0 = 305419896
        key1 = 591751049
        key2 = 878082192
        
        for c in password {
            if c == 0 {
                break
            }
            updateKeys(UInt8(bitPattern: c))
        }
    }
    
    func decode(_ c: UInt8) -> UInt8 {
        let cc = c ^ decryptByte()
        updateKeys(cc)
        return cc
    }
    
    func encode(_ c: UInt8) -> UInt8 {
        let t = decryptByte()
        updateKeys(c)
        return t ^ c
    }
    
    fileprivate func makeHeader(password: [CChar], crc32ForCrypting: UInt32?) -> [UInt8] {
        var header = [UInt8]()
        initKeys(password: password)
        for _ in 0 ..< (headerSize - 1) {
            let val = UInt8(truncatingBitPattern: arc4random())
            header.append(encode(val))
        }

        if let crc32ForCrypting = crc32ForCrypting {
            let val = UInt8(truncatingBitPattern: crc32ForCrypting >> 24)
            header.append(encode(val))
        } else {
            let val = UInt8(truncatingBitPattern: arc4random())
            header.append(encode(val))
        }
        
        return header
    }
    
}

class ZipCryptoStreamFactory: ZipEncryptionStreamFactory {

    private let password: [CChar]
    private let crc32: UInt32?
    
    init(password: String, encoding: String.Encoding = .ascii, crc32: UInt32? = nil) throws {
        guard let password = password.cString(using: encoding) else {
            throw ZipError.stringEncodingMismatch
        }
        self.password = password
        self.crc32 = crc32
    }
    
    func create(stream: IOStream) -> ZipEncryptionStream {
        return ZipCryptoStream(stream: stream, password: password, crc32: crc32)
    }
    
}

class ZipCryptoStream: ZipEncryptionStream {
    
    static var crc32Table: [UInt32]? = nil
    
    private var stream: IOStream
    private let password: [CChar]
    private let crc32: UInt32?

    private let zipCrypto: ZipCrypto
    
    var headerSize: Int {
        return zipCrypto.headerSize
    }
    
    private var writeBuffer: NSMutableData? = nil
    private var isFirst: Bool = true
    
    required init(
        stream: IOStream,
        password: [CChar],
        crc32: UInt32? = nil) {

        guard let crc32Table = ZipCryptoStream.crc32Table else {
            preconditionFailure("ZipCryptoStream.crc32Table must not be nil")
        }
        
        self.stream = stream
        self.password = password
        self.crc32 = crc32
        
        self.zipCrypto = ZipCrypto(crc32Table: crc32Table)
    }

    var canRead: Bool {
        return stream.canRead
    }
    
    var canSeek: Bool {
        return stream.canSeek
    }
    
    var canWrite: Bool {
        return stream.canWrite
    }
    
    var position: UInt64 {
        return stream.position
    }
    
    func close() -> Bool {
        return stream.close()
    }
    
    func read(buffer: UnsafeMutableRawPointer, maxLength len: Int) -> Int {
        if isFirst {
            isFirst = false
            
            zipCrypto.initKeys(password: password)
            var header = [UInt8](repeating: 0, count: zipCrypto.headerSize)
            _ = stream.read(buffer: &header, maxLength: header.count)
            for b in header {
                _ = zipCrypto.decode(b)
            }
        }
        
        let count = stream.read(buffer: buffer, maxLength: len)
        if count > 0 {
            let bytes = buffer.assumingMemoryBound(to: UInt8.self)
            for i in 0 ..< count {
                bytes[i] = zipCrypto.decode(bytes[i])
            }
        }
        return count
    }
    
    func seek(offset: Int, origin: SeekOrigin) -> Int {
        return stream.seek(offset: offset, origin: origin)
    }
    
    func write(buffer: UnsafeRawPointer, maxLength len: Int) -> Int {
        if isFirst {
            isFirst = false
            
            var header = zipCrypto.makeHeader(password: password, crc32ForCrypting: crc32)
            _  = stream.write(buffer: &header, maxLength: zipCrypto.headerSize)
        }
        
        if len > 0 {
            let originalBytes = buffer.assumingMemoryBound(to: UInt8.self)
            
            if writeBuffer == nil {
                writeBuffer = NSMutableData()
            }
            writeBuffer!.length = len
            let bytes = writeBuffer!.mutableBytes.assumingMemoryBound(to: UInt8.self)
            
            for i in 0 ..< len {
                bytes[i] = zipCrypto.encode(originalBytes[i])
                //print("\(originalBytes[i]) ==> \(bytes[i])")
            }
            
            return stream.write(buffer: bytes, maxLength: len)
        } else {
            return stream.write(buffer: buffer, maxLength: len)
        }
    }
    
}
