//
//  CZDecompress.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/15.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZDecompress.h"

struct CZDecompress {
    
    swift_int_t method;

    CZStreamRef stream;
    CZDecompressReadFunc read;
    CZDecompressReleaseFunc release;
    void * extraData;
    
    uint32_t crc32;

};

CZDecompressRef CZDecompressCreate(CZStreamRef stream, CZDecompressReadFunc read, CZDecompressReleaseFunc release) {
    CZDecompressRef obj = calloc(1, sizeof(struct CZDecompress));
    obj->stream = stream;
    obj->read = read;
    obj->release = release;
    return obj;
}

void CZDecompressRelease(CZDecompressRef obj) {
    if (obj->release) {
        obj->release(obj->extraData);
    }
    free(obj);
}

void CZDecompressSetMethod(CZDecompressRef obj, swift_int_t method) {
    obj->method = method;
}

swift_int_t CZDecompressGetMethod(CZDecompressRef obj) {
    return obj->method;
}

CZStreamRef CZDecompressGetStream(const CZDecompressRef obj) {
    return obj->stream;
}

void * CZDecompressGetExtraData(const CZDecompressRef obj) {
    return obj->extraData;
}

void CZDecompressSetExtraData(CZDecompressRef obj, void * opaque) {
    obj->extraData = opaque;
}

swift_int_t CZDecompressRead(CZDecompressRef obj, uint8_t *buffer, swift_int_t length) {
    int64_t len = obj->read(obj->stream, buffer, length, obj->extraData);
    if (len > 0) {
        obj->crc32 = (uint32_t)crc32(obj->crc32, buffer, (uint32_t)len);
    }
    return len;
}

uint32_t CZDecompressGetCRC32(CZDecompressRef obj) {
    return obj->crc32;
}

CZDecompressRef CZDecompressDefaultFactory(CZEntryHeaderInfo info, CZStreamRef stream) {
    CZDecompressRef obj = NULL;
    switch (info.method) {
        case 0:
            obj = CZDecompressStoreCreate(stream, info.size);
            break;
        case Z_DEFLATED:
            obj = CZDecompressDeflateCreate(stream, CZDefaultBufferSize);
            break;
        default:
            // Unsupported compression method
            break;
    }
    if (obj) {
        obj->method = info.method;
    }
    return obj;
}
