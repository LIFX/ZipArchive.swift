//
//  CZData.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/17.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZData_internal.h"

struct CZData {
    
    uint8_t * bytes;
    swift_int_t length;
    swift_int_t capacity;

};

// MARK: - private

void _CZDataAllocate(CZDataRef obj, swift_int_t minCapacity) {
    if (minCapacity <= obj->capacity) {
        return;
    }
    
    swift_int_t newCapacity = 0;
    if (obj->capacity > 0) {
        newCapacity = obj->capacity * 2;
    } else {
        newCapacity = 1;
    }
    while (newCapacity < minCapacity) {
        newCapacity *= 2;
    }
    
    uint8_t * newBytes;
    if (obj->bytes == NULL) {
        newBytes = malloc(newCapacity);
    } else {
        newBytes = realloc(obj->bytes, newCapacity);
        if (newBytes == NULL) {
            free(obj->bytes);
        }
    }
    
    obj->capacity = newCapacity;
    obj->bytes = newBytes;
}

// MARK: - public

CZDataRef CZDataCreate(void) {
    CZDataRef obj = calloc(1, sizeof(struct CZData));
    return obj;
}

void CZDataRelease(CZDataRef obj) {
    if (obj->bytes) {
        free(obj->bytes);
        obj->bytes = NULL;
    }
    obj->length = 0;
    obj->capacity = 0;
    free(obj);
}

void CZDataAppendWithBytes(CZDataRef obj, const uint8_t * bytes, swift_int_t length) {
    swift_int_t newLength = obj->length + length;
    _CZDataAllocate(obj, newLength);
    memcpy(obj->bytes + obj->length, bytes, length);
    obj->length = newLength;
}

void CZDataAppendWithData(CZDataRef obj, const CZDataRef other) {
    CZDataAppendWithBytes(obj, other->bytes, other->length);
}

const uint8_t * CZDataGetBytes(const CZDataRef obj) {
    _CZDataAllocate(obj, 1);
    return obj->bytes;
}

uint8_t * CZDataGetMutableBytes(CZDataRef obj) {
    _CZDataAllocate(obj, 1);
    return obj->bytes;
}

swift_int_t CZDataGetLength(const CZDataRef obj) {
    return obj->length;
}

void CZDataSetLength(CZDataRef obj, swift_int_t newLength) {
    _CZDataAllocate(obj, newLength);
    swift_int_t expandedSize = newLength - obj->length;
    if (expandedSize > 0) {
        memset(obj->bytes + obj->length, 0, expandedSize);
    }
    obj->length = newLength;
}
