//
//  CZUnzipEntry.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/15.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZUnzipEntry.h"

#include "CZTypes_internal.h"
#include "CZUnzip_internal.h"
#include "CZBinaryUtility_internal.h"

#include "CZDecompressStore.h"
#include "CZDecompressDeflate.h"

struct CZUnzipEntry {
    
    //CZStreamRef stream;
    //CZEntryHeaderRef globalHeader;
    
    CZUnzipRef unzip;

    CZEntryHeaderRef localHeader;
    size_t localHeaderSize;
    
    CZDecompressRef decompress;
    
    //CZDecompressFactoryFunc decompressFactory;
    //CZCryptoRef crypto;
    
};

// MARK: - private

static inline CZEntryHeaderRef _CZUnzipEntryReadLocalHeader(CZStreamRef stream, size_t headerOffset, size_t * headerSize) {
    CZEntryHeaderRef _header = CZEntryHeaderCreate();
    
    swift_int_t status;
    
    status = CZStreamSeek(stream, headerOffset, CZStreamSeekOriginBegin);
    
    size_t length = sizeof(CZEntryHeaderInfo) * 10; // バッファサイズは適当
    uint8_t * bytes = malloc(length);
    size_t readLen = CZStreamRead(stream, bytes, length);
    
    size_t offset = 0;
    
    uint32_t signature = CZBinaryReadUInt32LE(bytes, &offset);
    
    if (signature != CZLocalHeaderSignature) {
        return NULL;
    }
    
    CZEntryHeaderInfo headerInfo = {0};
    
    headerInfo.versionNeededToExtract = CZBinaryReadUInt16LE(bytes + offset, &offset);
    headerInfo.flags = CZBinaryReadUInt16LE(bytes + offset, &offset);
    headerInfo.method = CZBinaryReadUInt16LE(bytes + offset, &offset);
    headerInfo.time = CZBinaryReadUInt16LE(bytes + offset, &offset);
    headerInfo.date = CZBinaryReadUInt16LE(bytes + offset, &offset);
    headerInfo.crc = CZBinaryReadUInt32LE(bytes + offset, &offset);
    headerInfo.compressedSize = CZBinaryReadUInt32LE(bytes + offset, &offset);
    headerInfo.size = CZBinaryReadUInt32LE(bytes + offset, &offset);
    
    uint16_t fileNameLength = CZBinaryReadUInt16LE(bytes + offset, &offset);
    uint16_t extraFieldLength = CZBinaryReadUInt16LE(bytes + offset, &offset);
    
    CZEntryHeaderSetInfo(_header, headerInfo);
    
    size_t dataLength = fileNameLength + extraFieldLength;
    
    *headerSize = offset + dataLength;
    
    if (length < dataLength) {
        void * newBytes = realloc(bytes, dataLength);
        if (newBytes == NULL) {
            free(bytes);
        }
        bytes = newBytes;
        length = dataLength;
    }
    
    // TODO: dataLength が 0 読まずに buffer を free して終了??
    
    status = CZStreamSeek(stream, headerOffset + offset, CZStreamSeekOriginBegin);
    readLen = CZStreamRead(stream, bytes, dataLength);
    offset = 0;
    
    if (fileNameLength > 0) {
        //CZDataRef fileName = CZDataCreate();
        //CZDataAppendWithBytes(fileName, bytes + offset, fileNameLength);
        //CZEntryHeaderSetFileNameNoCopy(_header, fileName);
        CZEntryHeaderSetFileName(_header, (const char *)(bytes + offset), fileNameLength);
        
        offset += fileNameLength;
    }
    
    if (extraFieldLength > 0) {
        size_t o = 0;
        while (o < extraFieldLength) {
            if ((o + 4) > extraFieldLength) {
                // field length error!!
                break;
            }
            
            //CZExtraFieldRef extraField = CZExtraFieldCreate();
            
            //CZExtraFieldSetHeaderID(extraField, CZBinaryReadUInt16LE(bytes + offset + o, &o));
            uint16_t id = CZBinaryReadUInt16LE(bytes + offset + o, &o);
            
            uint16_t dataSize = CZBinaryReadUInt16LE(bytes + offset + o, &o);
            
            //CZDataRef data = CZDataCreate();
            //CZDataAppendWithBytes(data, bytes + offset + o, dataSize);
            //CZExtraFieldSetDataNoCopy(extraField, data);
            
            
            //CZEntryHeaderAppendExtraFieldNoCopy(_header, extraField);
            CZEntryHeaderAppendExtraField(_header, id, bytes + offset + o, dataSize);
            o += dataSize;
        }
        
        offset += extraFieldLength;
    }
    
    free(bytes);
    
    return _header;
}

// MARK: - internal

CZUnzipEntryRef CZUnzipEntryCreate(CZUnzipRef unzip) {
    CZUnzipEntryRef obj = calloc(1, sizeof(struct CZUnzipEntry));
    obj->unzip = unzip;
    return obj;
}

// MARK: - public

void CZUnzipEntryRelease(CZUnzipEntryRef obj) {
    if (obj->localHeader) {
        CZEntryHeaderRelease(obj->localHeader);
        obj->localHeader = NULL;
    }
    if (obj->decompress) {
        CZDecompressRelease(obj->decompress);
        obj->decompress = NULL;
    }
    free(obj);
}

void CZUnzipEntryOpen(CZUnzipEntryRef obj, const char * password, CZDecompressFactoryFunc decompressFactory) {
    CZEntryHeaderRef globalHeader = CZUnzipGetCurrentGlobalHeader(obj->unzip);
    CZEntryHeaderInfo globalInfo = CZEntryHeaderGetInfo(globalHeader);
    
    CZStreamRef stream = CZUnzipGetStream(obj->unzip);
    obj->localHeader = _CZUnzipEntryReadLocalHeader(stream, globalInfo.localHeaderOffset, &obj->localHeaderSize);
    
    if (decompressFactory == NULL) {
        decompressFactory = CZDecompressDefaultFactory;
    }
    obj->decompress = decompressFactory(globalInfo, stream);
    
    CZCryptoRef crypto = NULL;
    if (password) {
        if (globalInfo.compressedSize >= CZCryptoHeaderSize) {
            crypto = CZCryptoCreate(password);
            uint8_t cryptoHeader[CZCryptoHeaderSize];
            CZStreamRead(stream, cryptoHeader, CZCryptoHeaderSize);
            for (int32_t i = 0; i < CZCryptoHeaderSize; i += 1) {
                /*uint8_t x = */CZCryptoDecode(crypto, cryptoHeader[i]);
            }
        }
    }
    CZStreamAttachCrypto(stream, crypto);
}

void CZUnzipEntryClose(CZUnzipEntryRef obj) {
    CZStreamRef stream = CZUnzipGetStream(obj->unzip);
    CZCryptoRef crypto = CZStreamDetachCrypto(stream);
    if (crypto) {
        CZCryptoRelease(crypto);
    }
}

swift_int_t CZUnzipEntryRead(CZUnzipEntryRef obj, uint8_t * buffer, swift_int_t length) {
    return CZDecompressRead(obj->decompress, buffer, length);
}

uint32_t CZUnzipGetCRC32(CZUnzipEntryRef obj) {
    return CZDecompressGetCRC32(obj->decompress);
}

//const CZEntryHeaderRef CZUnzipEntryGetGlobalHeader(const CZUnzipEntryRef obj) {
//    return obj->globalHeader;
//}

const CZEntryHeaderRef CZUnzipEntryGetLocalHeader(const CZUnzipEntryRef obj) {
    return obj->localHeader;
}


