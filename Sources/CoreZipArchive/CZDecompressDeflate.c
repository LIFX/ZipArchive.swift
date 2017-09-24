//
//  CZDecompressDeflate.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/18.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZDecompressDeflate.h"

typedef struct {

    z_stream strm;
    bool isStreamEnd;
    swift_int_t bufferLength;
    uint8_t * buffer;

} CZDecompressDeflate;

swift_int_t _CZDecompressDeflateRead(CZStreamRef stream, uint8_t * buffer, swift_int_t length, void * opaque) {
    CZDecompressDeflate * extra = opaque;
    
    if (extra->isStreamEnd) {
        return 0;
    }

    extra->strm.next_out = (void *)buffer;
    extra->strm.avail_out = (uInt)length;

    int32_t status = Z_OK;
    while (status == Z_OK) {
        if (extra->strm.avail_in == 0) {
            // TODO: guard ...
            size_t size = CZStreamRead(stream, extra->buffer, extra->bufferLength);

            extra->strm.next_in = extra->buffer;
            extra->strm.avail_in = (uInt)size;
        }
        status = inflate(&extra->strm, Z_NO_FLUSH);
        if (status == Z_STREAM_END) {
            extra->isStreamEnd = true;
            break;
        }
        if (status != Z_OK) {
            // TODO:
            return -1;
        }
        if (extra->strm.avail_out == 0) {
            break;
        }
    }

    swift_int_t readBytes = length - extra->strm.avail_out;

    if (extra->isStreamEnd) {
        // TODO:
    }

    return readBytes;
}

void _CZDecompressDeflateRelease(void * opaque) {
    CZDecompressDeflate * extra = opaque;
    if (extra) {
        inflateEnd(&extra->strm);
        if (extra->buffer) {
            free(extra->buffer);
        }
        free(extra);
    }
}

CZDecompressRef CZDecompressDeflateCreate(CZStreamRef stream, swift_int_t bufferSize) {
    CZDecompressDeflate * extra = calloc(1, sizeof(CZDecompressDeflate));

    int32_t status = inflateInit2(&extra->strm, -MAX_WBITS);
    if (status != Z_OK) {
        free(extra);
        return NULL;
    }

    extra->isStreamEnd = false;
    extra->bufferLength = bufferSize;
    extra->buffer = malloc(extra->bufferLength);

    CZDecompressRef decompress = CZDecompressCreate(stream, _CZDecompressDeflateRead, _CZDecompressDeflateRelease);
    CZDecompressSetExtraData(decompress, extra);

    return decompress;
}
