//
//  CZStreamInMemory.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/17.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZStreamInMemory.h"

#include "CZStream_internal.h"

typedef struct {
    
    void * bytes;
    int64_t length;
    int64_t position;

} CZStreamInMemoryData;

// MARK: - private

static void _CZStreamInMemoryDataAllocate(CZStreamInMemoryData * data, int64_t size) {
    if (data->bytes == NULL) {
        data->bytes = malloc(size);
        data->length = size;
        data->position = 0;
        return;
    }
    if ((data->length - data->position) < size) {
        int64_t newLength = data->position + size;
        void * ptr = realloc(data->bytes, newLength);
        if (ptr == NULL) {
            // TODO: Error
            free(data->bytes);
        }
        data->bytes = ptr;
        data->length = newLength;
        return;
    }
}

swift_int_t _CZStreamInMemoryClose(void * opaque) {
    CZStreamInMemoryData * data = opaque;
    if (data->bytes) {
        free(data->bytes);
    }
    free(data);
    return 0;
}

size_t _CZStreamInMemoryRead(void * opaque, uint8_t * buffer, size_t count) {
    CZStreamInMemoryData * data = opaque;
    size_t size = MIN(count, data->length - data->position);
    memcpy(buffer, data->bytes + data->position, size);
    data->position += size;
    return size;
}

size_t _CZStreamInMemoryWrite(void * opaque, const uint8_t * buffer, size_t count) {
    CZStreamInMemoryData * data = opaque;
    _CZStreamInMemoryDataAllocate(data, (int64_t)count);
    memcpy(data->bytes + data->position, buffer, count);
    data->position += count;
    return count;
}

swift_int_t _CZStreamInMemorySeek(void * opaque, int64_t offset, CZStreamSeekOrigin origin) {
    CZStreamInMemoryData * data = opaque;
    int64_t maxOffset = 0;
    if (data->length > 0) {
        maxOffset = data->length - 1;
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
        return -1;
    }
    data->position = newOffset;
    return 0;
}

int64_t _CZStreamInMemoryTell(void * opaque) {
    CZStreamInMemoryData * data = opaque;
    return (int64_t)data->position;
}

// MARK: - public

CZStreamRef CZStreamCreateInMemory(void) {
    CZStreamInMemoryData * data = calloc(1, sizeof(CZStreamInMemoryData));

    CZStreamFunctions funcs = {0};
    
    funcs.open = NULL;
    funcs.close = _CZStreamInMemoryClose;
    funcs.read = _CZStreamInMemoryRead;
    funcs.write = _CZStreamInMemoryWrite;
    funcs.seek = _CZStreamInMemorySeek;
    funcs.tell = _CZStreamInMemoryTell;
    //funcs.eof = NULL;
    funcs.error = NULL;
    
    CZStreamRef stream = CZStreamCreate(funcs, data);
    CZStreamSetSubType(stream, CZStreamSubTypeInMemory);

    return stream;
}

swift_int_t CZStreamGetMemoryBufferLength(CZStreamRef obj) {
    if (CZStreamGetSubType(obj) != CZStreamSubTypeInMemory) {
        return 0;
    }
    CZStreamInMemoryData * data = CZStreamGetExtraData(obj);
    return data->length;
}

const void * _Nullable CZStreamGetMemoryBuffer(CZStreamRef obj) {
    if (CZStreamGetSubType(obj) != CZStreamSubTypeInMemory) {
        return NULL;
    }
    CZStreamInMemoryData * data = CZStreamGetExtraData(obj);
    return data->bytes;
}
