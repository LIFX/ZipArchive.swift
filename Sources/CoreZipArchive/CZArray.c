//
//  CZArray.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/16.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZArray_internal.h"

struct CZArray {
    
    swift_int_t count;
    swift_int_t capacity;
    void * (* data);
    
};

// MARK: - private

static inline void _CZArrayAllocate(CZArrayRef obj) {
    if (obj->count != obj->capacity) {
        return;
    }
    
    swift_int_t newCapacity = obj->capacity;
    if (newCapacity == 0) {
        newCapacity = 1;
    } else {
        newCapacity *= 2;
    }
    
    void * newData = NULL;
    if (obj->data == NULL) {
        newData = malloc(sizeof(void *) * newCapacity);
    } else {
        newData = realloc(obj->data, sizeof(void *) * newCapacity);
        if (newData == NULL) {
            free(obj->data);
        }
    }
    
    obj->capacity = newCapacity;
    obj->data = newData;
}

// MARK: - public

CZArrayRef CZArrayCreate(void) {
    CZArrayRef array = calloc(1, sizeof(struct CZArray));
    return array;
}

void CZArrayRelease(CZArrayRef obj) {
    if (obj->data) {
        free(obj->data);
        obj->data = NULL;
    }
    free(obj);
}

swift_int_t CZArrayPushBack(CZArrayRef obj, void * element) {
    _CZArrayAllocate(obj);
    obj->data[obj->count] = element;
    obj->count += 1;
    return obj->count;
}

void * CZArrayPopBack(CZArrayRef obj) {
    if (obj->count == 0) {
        return NULL;
    }
    obj->count -= 1;
    return obj->data[obj->count];
}

void * CZArrayGetElementAtIndex(const CZArrayRef obj, swift_int_t index) {
    if (obj->count <= index) {
        // Out of range
        return NULL;
    }
    return obj->data[index];
}

swift_int_t CZArrayGetCount(const CZArrayRef obj) {
    return obj->count;
}
