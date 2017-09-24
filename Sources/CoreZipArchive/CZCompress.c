//
//  CZCompress.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/16.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZCompress.h"

struct CZCompress {
    
    swift_int_t method;

    CZStreamRef stream;
    uint32_t crc32;
    void * extraData;
    
    CZCompressWriteFunc write;
    CZCompressReleaseFunc release;
    CZCompressGetOriginalSizeFunc getOriginalSize;
    CZCompressGetCompressedSizeFunc getCompressedSize;

};

CZCompressRef CZCompressCreate(CZStreamRef stream, CZCompressWriteFunc write, CZCompressReleaseFunc release, CZCompressGetOriginalSizeFunc getOriginalSize, CZCompressGetCompressedSizeFunc getCompressedSize) {
    CZCompressRef obj = calloc(1, sizeof(struct CZCompress));
    obj->stream = stream;
    obj->write = write;
    obj->release = release;
    obj->getOriginalSize = getOriginalSize;
    obj->getCompressedSize = getCompressedSize;
    return obj;
}

void CZCompressRelease(CZCompressRef obj) {
    if (obj->release) {
        obj->release(obj->extraData);
    }
    free(obj);
}

void CZCompressSetMethod(CZCompressRef _Nonnull obj, swift_int_t method) {
    obj->method = method;
}

swift_int_t CZCompressGetMethod(CZCompressRef _Nonnull obj) {
    return obj->method;
}

CZStreamRef CZCompressGetStream(const CZCompressRef obj) {
    return obj->stream;
}

void * CZCompressGetExtraData(const CZCompressRef obj) {
    return obj->extraData;
}

void CZCompressSetExtraData(CZCompressRef obj, void * opaque) {
    obj->extraData = opaque;
}

swift_int_t CZCompressWrite(CZCompressRef obj, const uint8_t * buffer, swift_int_t length) {
    obj->crc32 = (uint32_t)crc32(obj->crc32, buffer, (uint32_t)length);
    return obj->write(obj->stream, buffer, length, obj->extraData);
}

swift_int_t CZCompressGetOriginalSize(CZCompressRef obj) {
    return obj->getOriginalSize(obj->extraData);
}

swift_int_t CZCompressGetCompressedSize(CZCompressRef obj) {
    return obj->getCompressedSize(obj->extraData);
}

uint32_t CZCompressGetCRC32(CZCompressRef obj) {
    return obj->crc32;
}

CZCompressRef CZCompressDefaultFactory(swift_int_t method, swift_int_t level, CZStreamRef stream) {
    CZCompressRef obj = NULL;
    switch (method) {
        case 0:
            obj = CZCompressStoreCreate(stream);
            break;
        case Z_DEFLATED:
            obj = CZCompressDeflateCreate(stream, level, CZDefaultBufferSize);
            break;
        default:
            // Unsupported compression method
            break;
    }
    if (obj) {
        obj->method = method;
    }
    return obj;
}
