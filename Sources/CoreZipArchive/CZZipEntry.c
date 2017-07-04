//
//  CZZipEntry.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/16.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZZipEntry.h"

#include "CZZip_internal.h"
#include "CZData_internal.h"
#include "CZBinaryUtility_internal.h"

struct CZZipEntry {

    CZZipRef zip;
    
    CZEntryHeaderRef header;
    int64_t headerOffset;
    
    CZCompressRef compress;
    
};

// MARK: - private

static void _CZZipEntryWriteHeader(CZStreamRef stream, CZEntryHeaderRef header) {
    const uint16_t fileNameLength = CZEntryHeaderGetFileNameLength(header);
    char fileName[fileNameLength + 1];
    CZEntryHeaderGetFileName(header, fileName);
    
    uint16_t extraFieldLength = 0;
    CZDataRef extraFieldData = CZDataCreate();
    
    const swift_int_t count = CZEntryHeaderGetExtraFieldCount(header);
    for (swift_int_t i = 0; i < count; i += 1) {
        const CZExtraFieldRef extraField = CZEntryHeaderGetExtraFieldAtIndex(header, i);

        uint16_t headerID = CZExtraFieldGetHeaderID(extraField);
        const size_t dataSize = CZExtraFieldGetDataLength(extraField);
        const uint8_t * data = CZExtraFieldGetData(extraField);

        const size_t bufferSize = sizeof(uint16_t) + sizeof(uint16_t) + dataSize;
        uint8_t buffer[bufferSize];
        
        size_t offset = 0;
        offset += CZBinaryWriteUInt16LE(buffer + offset, headerID);
        offset += CZBinaryWriteUInt16LE(buffer + offset, dataSize);
        memcpy(buffer + offset, data, dataSize);
        offset += dataSize;
        
        CZDataAppendWithBytes(extraFieldData, buffer, offset);
    }
    
    CZEntryHeaderInfo headerInfo = CZEntryHeaderGetInfo(header);

    size_t offset = 0;
    
    {
        uint8_t buffer[sizeof(CZEntryHeaderInfo) * 10]; // バッファサイズは適当
        offset += CZBinaryWriteUInt32LE(buffer + offset, CZLocalHeaderSignature);
        offset += CZBinaryWriteUInt16LE(buffer + offset, headerInfo.versionNeededToExtract);
        offset += CZBinaryWriteUInt16LE(buffer + offset, headerInfo.flags);
        offset += CZBinaryWriteUInt16LE(buffer + offset, headerInfo.method);
        offset += CZBinaryWriteUInt16LE(buffer + offset, headerInfo.time);
        offset += CZBinaryWriteUInt16LE(buffer + offset, headerInfo.date);
        offset += CZBinaryWriteUInt32LE(buffer + offset, headerInfo.crc); // 0
        offset += CZBinaryWriteUInt32LE(buffer + offset, headerInfo.compressedSize); // 0
        offset += CZBinaryWriteUInt32LE(buffer + offset, headerInfo.size); // 0
        
        offset += CZBinaryWriteUInt16LE(buffer + offset, fileNameLength);

        if (extraFieldData) {
            extraFieldLength = CZDataGetLength(extraFieldData);
        }
        offset += CZBinaryWriteUInt16LE(buffer + offset, extraFieldLength);
        
        CZStreamWrite(stream, buffer, offset);
    }
    
    if (fileNameLength > 0) {
        //const uint8_t * bytes = CZDataGetBytes(fileNameData);
        CZStreamWrite(stream, (const uint8_t *)fileName, fileNameLength);
    }

    if (extraFieldLength > 0) {
        const uint8_t * bytes = CZDataGetBytes(extraFieldData);
        CZStreamWrite(stream, bytes, extraFieldLength);
    }    
}

//static void _CZZipEntryWriteFooter(CZStreamRef stream, CZDataDescriptor footer) {
//    uint8_t buffer[sizeof(uint32_t) * 3];
//    size_t offset = 0;
//    
//    offset += CZBinaryWriteUInt32LE(buffer + offset, footer.crc32);
//    offset += CZBinaryWriteUInt32LE(buffer + offset, footer.compressedSize);
//    offset += CZBinaryWriteUInt32LE(buffer + offset, footer.uncompressedSize);
//    
//    CZStreamWrite(stream, buffer, offset);
//}

static void _CZZipEntryWriteGlobalHeader(CZStreamRef tempStream, size_t headerOffset, CZEntryHeaderRef localHeader, CZDataDescriptor footer) {
    
    const uint16_t fileNameLength = CZEntryHeaderGetFileNameLength(localHeader);
    char fileName[fileNameLength + 1];
    CZEntryHeaderGetFileName(localHeader, fileName);
    //CZDataRef fileName = CZLocalHeaderGetFileName(localHeader);
    //uint16_t fileNameLength = CZDataGetCount(fileName);

    {
        uint8_t buffer[128];
        size_t offset = 0;
        
        offset += CZBinaryWriteUInt32LE(buffer + offset, CZGlobalHeaderSignature);
        
        CZEntryHeaderInfo localInfo = CZEntryHeaderGetInfo(localHeader);
        //offset += CZBinaryWriteUInt16LE(buffer + offset, 20);
        
        *(buffer + offset) = (uint8_t)localInfo.versionNeededToExtract;
        offset += 1;
        
        *(buffer + offset) = 0;
        offset += 1;
        
        offset += CZBinaryWriteUInt16LE(buffer + offset, localInfo.versionNeededToExtract);
        offset += CZBinaryWriteUInt16LE(buffer + offset, localInfo.flags);
        offset += CZBinaryWriteUInt16LE(buffer + offset, localInfo.method);
        offset += CZBinaryWriteUInt16LE(buffer + offset, localInfo.time);
        offset += CZBinaryWriteUInt16LE(buffer + offset, localInfo.date);
        
        offset += CZBinaryWriteUInt32LE(buffer + offset, footer.crc32);
        offset += CZBinaryWriteUInt32LE(buffer + offset, footer.compressedSize);
        offset += CZBinaryWriteUInt32LE(buffer + offset, footer.uncompressedSize);
        
        offset += CZBinaryWriteUInt16LE(buffer + offset, fileNameLength);

        // extraFieldLength
        //size_t extraFieldLengthOffset = offset;
        offset += CZBinaryWriteUInt16LE(buffer + offset, 0);

        // fileCommentLength
        offset += CZBinaryWriteUInt16LE(buffer + offset, 0);
        
        // diskNumberStrat
        offset += CZBinaryWriteUInt16LE(buffer + offset, 0);
        
        // internalFileAttributes
        offset += CZBinaryWriteUInt16LE(buffer + offset, 0);

        // externalFileAttributes
        offset += CZBinaryWriteUInt32LE(buffer + offset, 0);
        
        // relativeOffsettOfLocalHeader
        offset += CZBinaryWriteUInt32LE(buffer + offset, (uint32_t)headerOffset);
        
        size_t len = CZStreamWrite(tempStream, buffer, offset);
        if (len != offset) {
            // TODO: ERROR
            
        }
    }
    
    // fileName
    {
        //const uint8_t * fileNameData = CZDataGetBytes(fileName);
        size_t len = CZStreamWrite(tempStream, (const uint8_t *)fileName, fileNameLength);
        if (len != fileNameLength) {
            // TODO: error
        }
    }

    // extraField
    {
        const swift_int_t extraFieldCount = CZEntryHeaderGetExtraFieldCount(localHeader);
        for (swift_int_t i = 0; i < extraFieldCount; i += 1) {
            const CZExtraFieldRef extraField = CZEntryHeaderGetExtraFieldAtIndex(localHeader, i);
            uint16_t id = CZExtraFieldGetHeaderID(extraField);
//            CZDataRef data = CZExtraFieldGetData(extraField);
//            uint16_t dataCount = CZDataGetCount(data);
//            const uint8_t * dataBytes = CZDataGetBytes(data);
            
            uint16_t dataSize = CZExtraFieldGetDataLength(extraField);
            const uint8_t * data = CZExtraFieldGetData(extraField);

            uint8_t buffer[sizeof(uint16_t) * 2];
            CZBinaryWriteUInt16LE(buffer, id);
            CZBinaryWriteUInt16LE(buffer + 2, dataSize);
            size_t len = CZStreamWrite(tempStream, buffer, 4);
            if (len != 4) {
                // TODO: error
            }
            
            len = CZStreamWrite(tempStream, data, dataSize);
            if (len != dataSize) {
                // TODO: error
            }
        }
    }
    
    // fileComment: Nothing
}

// MARK: - internal

//CZZipEntryRef CZZipEntryCreate(CZZipRef zip, CZLocalHeaderRef header, CZCompressRef compress, CZCryptoRef crypto, uint32_t crc32) {
CZZipEntryRef CZZipEntryCreate(CZZipRef zip, const char * entryName, CZDateTime time, swift_int_t method, swift_int_t level, CZCompressFactoryFunc compressFactory, CZCryptoRef crypto, uint32_t crc32) {
    CZZipEntryRef obj = calloc(1, sizeof(struct CZZipEntry));

    obj->zip = zip;
    
    CZStreamRef stream = CZZipGetStream(zip);
    
    CZEntryHeaderInfo localInfo = {0};
    localInfo.versionNeededToExtract = 20;
    localInfo.flags = (crypto) ? 1 : 0;
    localInfo.method = method;
    localInfo.time = CZDateTimeGetZipTime(time);
    localInfo.date = CZDateTimeGetZipDate(time);
    localInfo.crc = 0;
    localInfo.compressedSize = 0;
    localInfo.size = 0;
    
    //CZDataRef name = CZDataCreate();
    //CZDataAppendWithBytes(name, (const void *)entryName, strlen(entryName));
    
    CZEntryHeaderRef header = CZEntryHeaderCreate();
    CZEntryHeaderSetInfo(header, localInfo);
    CZEntryHeaderSetFileName(header, entryName, strlen(entryName));
    //CZEntryHeaderSetFileNameNoCopy(header, name);
    
    obj->header = header;
    
    //CZLocalHeaderInfo info = CZLocalHeaderGetInfo(header);
    //CZStreamRef stream = CZZipGetStream(zip);
    obj->compress = compressFactory(method, level, stream);
    
    CZStreamSeek(stream, 0, CZStreamSeekOriginEnd);
    obj->headerOffset = CZStreamTell(stream);
    _CZZipEntryWriteHeader(stream, header);

    if (crypto) {
        uint8_t cryptoHeader[CZCryptoHeaderSize];
        CZCryptoMakeHeader(crypto, crc32, cryptoHeader);
        CZStreamWrite(stream, cryptoHeader, CZCryptoHeaderSize);
    }
    CZStreamAttachCrypto(stream, crypto);
    
    return obj;
}

// MARK: - public

void CZZipEntryRelease(CZZipEntryRef obj) {
    CZEntryHeaderRelease(obj->header);
    obj->header = NULL;
    CZCompressRelease(obj->compress);
    obj->compress = NULL;
    free(obj);
}

swift_int_t CZZipEntryWrite(CZZipEntryRef obj, const uint8_t * buffer, swift_int_t length) {
    return CZCompressWrite(obj->compress, buffer, length);
}

uint32_t CZZipGetCRC32(CZZipEntryRef obj) {
    return CZCompressGetCRC32(obj->compress);
}

void CZZipEntryClose(CZZipEntryRef obj) {
    CZDataDescriptor footer = {0};
    footer.crc32 = CZCompressGetCRC32(obj->compress);
    footer.compressedSize = (uint32_t)CZCompressGetCompressedSize(obj->compress);
    footer.uncompressedSize = (uint32_t)CZCompressGetOriginalSize(obj->compress);

    CZStreamRef stream = CZZipGetStream(obj->zip);
    CZCryptoRef crypto = CZStreamDetachCrypto(stream);
    if (crypto) {
        CZCryptoRelease(crypto);
        footer.compressedSize += CZCryptoHeaderSize;
    }
    
    uint8_t buffer[12];
    size_t offset = 0;
    offset += CZBinaryWriteUInt32LE(buffer + offset, footer.crc32);
    offset += CZBinaryWriteUInt32LE(buffer + offset, footer.compressedSize);
    offset += CZBinaryWriteUInt32LE(buffer + offset, footer.uncompressedSize);
    CZStreamSeek(stream, obj->headerOffset + 4 + 2 + 2 + 2 + 2 + 2, CZStreamSeekOriginBegin);
    CZStreamWrite(stream, buffer, offset);
    
    CZStreamSeek(stream, 0, CZStreamSeekOriginEnd);
    //_CZZipEntryWriteFooter(stream, footer);
    
    // Global Header を書き出しておく
    //CZStreamRef tempStream = CZZipGetTempStream(obj->tmp);
    CZStreamRef tempStream = CZZipGetTempStream(obj->zip);
    _CZZipEntryWriteGlobalHeader(tempStream, obj->headerOffset, obj->header, footer);
}
