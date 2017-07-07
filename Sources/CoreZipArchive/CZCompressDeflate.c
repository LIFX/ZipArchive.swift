//
//  CZCompressDeflate.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/18.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZCompressDeflate.h"

typedef struct {

    z_stream strm;
    swift_int_t level;
    bool isStreamEnd;
    swift_int_t bufferLength;
    uint8_t * buffer;

} CZCompressDeflate;

// MARK: - private

swift_int_t _CZCompressDeflateWrite(CZStreamRef stream, const uint8_t * buffer, swift_int_t length, void * opaque) {

    CZCompressDeflate * extra = opaque;
    
    if (extra->isStreamEnd) {
        return 0;
    }

    extra->strm.next_in = (void *)buffer;
    extra->strm.avail_in = (uInt)length;

    extra->strm.next_out = extra->buffer;
    extra->strm.avail_out = (uInt)extra->bufferLength;

    int32_t flush = (length == 0) ? Z_FINISH : Z_NO_FLUSH;
    int32_t status = Z_OK;
    while (status == Z_OK) {
#if HAS_DEFLATE
        // FIXME:
        status = deflate(&extra->strm, flush);
#endif
        if (status == Z_STREAM_END) {
            extra->isStreamEnd = true;
            break;
        }
        if (status != Z_OK) {
            // TODO:
            return -1;
        }
        if (extra->strm.avail_out == 0) {
            size_t len = CZStreamWrite(stream, extra->buffer, extra->bufferLength);
            if (len != extra->bufferLength) {
                // TODO:
                return -1;
            }
            extra->strm.next_out = extra->buffer;
            extra->strm.avail_out = (uInt)extra->bufferLength;
        } else {
            break;
        }
    }

    swift_int_t count = extra->bufferLength - extra->strm.avail_out;
    if (count != 0) {
        size_t len = CZStreamWrite(stream, extra->buffer, count);
        if (len != count) {
            // TODO:
            return -1;
        }
    }

    swift_int_t writeBytes = length - extra->strm.avail_in;
    
    return writeBytes;
}

void _CZCompressDeflateRelease(void * opaque) {
    CZCompressDeflate * extra = opaque;
    if (extra) {
#if HAS_DEFLATE
        // FIXME:
        deflateEnd(&extra->strm);
#endif
        if (extra->buffer) {
            free(extra->buffer);
        }
        free(extra);
    }
}

swift_int_t _CZCompressDeflateGetOriginalSize(void * opaque) {
    CZCompressDeflate * extra = opaque;
    return extra->strm.total_in;
}

swift_int_t _CZCompressDeflateGetCompressedSize(void * opaque) {
    CZCompressDeflate * extra = opaque;
    return extra->strm.total_out;
}

// MARK: - public

CZCompressRef CZCompressDeflateCreate(CZStreamRef stream, swift_int_t level, swift_int_t bufferSize) {
    CZCompressDeflate * extra = calloc(1, sizeof(CZCompressDeflate));
#if HAS_DEFLATE
    // FIXME:
    deflateInit2(&extra->strm, level, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
#endif
    extra->level = level;
    extra->isStreamEnd = false;
    extra->bufferLength = bufferSize;
    extra->buffer = malloc(extra->bufferLength);

    CZCompressRef compress = CZCompressCreate(stream, _CZCompressDeflateWrite, _CZCompressDeflateRelease, _CZCompressDeflateGetOriginalSize, _CZCompressDeflateGetCompressedSize);
    CZCompressSetExtraData(compress, extra);

    return compress;
}

