//
//  CZZip.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/15.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZZip.h"
#include "CZTypes.h"
#include "CZStreamInMemory.h"

#include "CZZipEntry_internal.h"
#include "CZBinaryUtility_internal.h"

struct CZZip {
    
    CZStreamRef stream;
    
    CZStreamRef tempStream;
    bool closeTempStreamWhenRelease;
    
    swift_int_t entryCount;
    
};

// MARK: - internal

CZStreamRef CZZipGetStream(const CZZipRef obj) {
    return obj->stream;
}

CZStreamRef CZZipGetTempStream(const CZZipRef obj) {
    return obj->tempStream;
}

// MARK: - public

CZZipRef CZZipCreate(CZStreamRef stream) {
    CZStreamRef tempStream = CZStreamCreateInMemory();
    return CZZipCreateWithTemporaryStream(stream, tempStream, true);
}

CZZipRef CZZipCreateWithTemporaryStream(CZStreamRef stream, CZStreamRef tempStream, bool closeTempStreamWhenRelease) {
    CZZipRef zip = calloc(1, sizeof(struct CZZip));
    zip->stream = stream;
    zip->tempStream = tempStream;
    zip->closeTempStreamWhenRelease = closeTempStreamWhenRelease;
    return zip;
}

void CZZipRelease(CZZipRef obj) {
//    if (obj->stream) {
//        CZStreamRelease(obj->stream);
//    }
    if (obj->closeTempStreamWhenRelease) {
        if (obj->tempStream) {
            CZStreamClose(obj->tempStream);
            CZStreamRelease(obj->tempStream);
        }
    }
    obj->stream = NULL;
    obj->tempStream = NULL;
    free(obj);
}

CZZipEntryRef CZZipAppendEntry(CZZipRef obj, const char * entryName, swift_int_t method, CZDateTime time, swift_int_t level, CZCompressFactoryFunc compressFactory, const char * password, uint32_t crc32) {
    
    if (compressFactory == NULL) {
        compressFactory = CZCompressDefaultFactory;
    }
    
    CZCryptoRef crypto = NULL;
    if (password) {
        crypto = CZCryptoCreate(password);
    }
    
    CZZipEntryRef entry = CZZipEntryCreate(obj, entryName, time, method, level, compressFactory, crypto, crc32);
    
    obj->entryCount += 1;
    
    return entry;
}

void CZZipClose(CZZipRef obj) {
    // Write Global Header
    const int32_t bufferSize = 1024;
    uint8_t buffer[bufferSize];
    
    CZStreamSeek(obj->stream, 0, CZStreamSeekOriginEnd);
    CZStreamSeek(obj->tempStream, 0, CZStreamSeekOriginBegin);
    
    uint32_t centralDirectorySize = 0;
    int64_t centralDirectoryOffset = CZStreamTell(obj->stream);
    
    while (1) {
        uint64_t rlen = CZStreamRead(obj->tempStream, buffer, bufferSize);
        //if (rlen < 0) {
            // TODO: error
        //    break;
        //}
        if (rlen == 0) {
            break;
        }
        centralDirectorySize += rlen;
        /*uint64_t wlen = */CZStreamWrite(obj->stream, buffer, rlen);
    }
    
    // Write End Of Global Header
    size_t offset = 0;
    offset += CZBinaryWriteUInt32LE(buffer + offset, CZEndOfCentralDirectorySignature);
    offset += CZBinaryWriteUInt16LE(buffer + offset, 0);
    offset += CZBinaryWriteUInt16LE(buffer + offset, 0);
    uint16_t entryCount = obj->entryCount;
    offset += CZBinaryWriteUInt16LE(buffer + offset, entryCount);
    offset += CZBinaryWriteUInt16LE(buffer + offset, entryCount);
    offset += CZBinaryWriteUInt32LE(buffer + offset, centralDirectorySize);
    offset += CZBinaryWriteUInt32LE(buffer + offset, (uint32_t)centralDirectoryOffset);
    offset += CZBinaryWriteUInt16LE(buffer + offset, 0);
    
    /*uint64_t wlen = */CZStreamWrite(obj->stream, buffer, offset);
    
    // FIXME: 本当はここで閉じたいが、テストで落ちる
    // NULL PTR を realloc しようとしたと言われる
    // 結論；stream は外部から渡されるものなので、閉じなくて良い
    // Close Stream
    //CZStreamRelease(obj->stream);
    //obj->stream = NULL;
}

swift_int_t CZZipGetEntryCount(CZZipRef obj) {
    return obj->entryCount;
}
