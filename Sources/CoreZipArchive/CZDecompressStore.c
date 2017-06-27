//
//  CZDecompressStore.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/18.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZDecompressStore.h"

typedef struct {

    swift_int_t fileSize;
    swift_int_t totalReadLength;

} CZDecompressStore;

swift_int_t _CZDecompressStoreRead(CZStreamRef stream, uint8_t * buffer, swift_int_t length, void * opaque) {
    CZDecompressStore * extra = opaque;
    swift_int_t len = MIN(length, extra->fileSize - extra->totalReadLength);
    if (len < 0) {
        return -1;
    }
    size_t readBytes = CZStreamRead(stream, buffer, (size_t)len);
    //if (readBytes > 0) {
        extra->totalReadLength += readBytes;
    //}
    return readBytes;
}

void _CZDecompressStoreRelease(void * opaque) {
    CZDecompressStore * extra = opaque;
    if (extra) {
        free(extra);
    }
}

CZDecompressRef CZDecompressStoreCreate(CZStreamRef stream, swift_int_t fileSize) {
    CZDecompressStore * extra = calloc(1, sizeof(CZDecompressStore));
    extra->fileSize = fileSize;
    extra->totalReadLength = 0;

    CZDecompressRef decompress = CZDecompressCreate(stream, _CZDecompressStoreRead, _CZDecompressStoreRelease);
    CZDecompressSetExtraData(decompress, extra);

    return decompress;
}
