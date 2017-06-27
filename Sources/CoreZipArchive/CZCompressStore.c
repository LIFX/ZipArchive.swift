//
//  CZCompressStore.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/18.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZCompressStore.h"

typedef struct {
    
    swift_int_t totalSize;
    
} CZCompressStore;

// MARK: - private

swift_int_t _CZCompressStoreWrite(CZStreamRef stream, const uint8_t * buffer, swift_int_t length, void * opaque) {
    if (length == 0) {
        return 0;
    }
    size_t len = CZStreamWrite(stream, buffer, length);
    CZCompressStore * extra = opaque;
    extra->totalSize += len;
    return len;
}

void _CZCompressStoreRelease(void * opaque) {
    CZCompressStore * extra = opaque;
    if (extra) {
        free(extra);
    }
}

swift_int_t _CZCompressStoreGetOriginalSize(void * opaque) {
    CZCompressStore * extra = opaque;
    return extra->totalSize;
}

swift_int_t _CZCompressStoreGetCompressedSize(void * opaque) {
    CZCompressStore * extra = opaque;
    return extra->totalSize;
}

// MARK: - public

CZCompressRef CZCompressStoreCreate(CZStreamRef stream) {
    CZCompressRef compress = CZCompressCreate(stream, _CZCompressStoreWrite, _CZCompressStoreRelease, _CZCompressStoreGetOriginalSize, _CZCompressStoreGetCompressedSize);
    
    CZCompressStore * extra = calloc(1, sizeof(CZCompressStore));
    CZCompressSetExtraData(compress, extra);
    
    return compress;
}

