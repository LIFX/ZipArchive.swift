//
//  CZStreamBuffer.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/06/17.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZStreamBuffer.h"

#include "CZStream_internal.h"

typedef struct {

    void * buffer;
    size_t capacity;
    size_t position;
    
} CZStreamBufferData;

swift_int_t _CZStreamBufferClose(void * opaque) {
    CZStreamBufferData * data = opaque;
    data->buffer = NULL;
    data->capacity = 0;
    free(data);
    return 0;
}

size_t _CZStreamBufferRead(void * opaque, uint8_t * buffer, size_t count) {
    CZStreamBufferData * data = opaque;
    size_t size = MIN(count, data->capacity - data->position);
    if (size != 0) {
        memcpy(buffer, data->buffer + data->position, size);
        data->position += size;
    }
    return size;
}

size_t _CZStreamBufferWrite(void * opaque, const uint8_t * buffer, size_t count) {
    CZStreamBufferData * data = opaque;
    size_t size = MIN(count, data->capacity - data->position);
    memcpy(data->buffer + data->position, buffer, size);
    data->position += count;
    return count;
}

swift_int_t _CZStreamBufferSeek(void * opaque, int64_t offset, CZStreamSeekOrigin origin) {
    CZStreamBufferData * data = opaque;
    int64_t maxOffset = 0;
    if (data->capacity > 0) {
        maxOffset = data->capacity - 1;
    }
    int64_t newOffset = 0;
    switch (origin) {
        case CZStreamSeekOriginBegin:
            newOffset = offset;
            break;
        case CZStreamSeekOriginCurrent:
            newOffset = data->position + offset;
            break;
        case CZStreamSeekOriginEnd:
            newOffset = maxOffset + offset;
            break;
        default:
            return -1;
    }
    if (newOffset < 0 || newOffset > maxOffset) {
        //EINVAL
        return -1;
    }
    data->position = newOffset;
    return 0;
}

int64_t _CZStreamBufferTell(void * opaque) {
    CZStreamBufferData * data = opaque;
    return (int64_t)data->position;
}

// MARK: - public

CZStreamRef CZStreamCreateWithBuffer(void * buffer, size_t capacity) {
    CZStreamBufferData * data = calloc(1, sizeof(CZStreamBufferData));
    data->buffer = buffer;
    data->capacity = capacity;
    
    CZStreamFunctions funcs = {0};
    
    funcs.open = NULL;
    funcs.close = _CZStreamBufferClose;
    funcs.read = _CZStreamBufferRead;
    funcs.write = _CZStreamBufferWrite;
    funcs.seek = _CZStreamBufferSeek;
    funcs.tell = _CZStreamBufferTell;
    funcs.error = NULL;
    
    CZStreamRef stream = CZStreamCreate(funcs, data);
    CZStreamSetSubType(stream, CZStreamSubTypeBuffer);
    
    return stream;
}
