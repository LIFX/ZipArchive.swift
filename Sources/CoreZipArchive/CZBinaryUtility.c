//
//  CZBinaryUtility.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/06/18.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZBinaryUtility_internal.h"

uint8_t CZBinaryReadUInt8(const uint8_t * bytes, size_t * readSize) {
    const size_t size = sizeof(uint8_t);
    if (readSize) {
        *readSize += size;
    }
    return bytes[0];
}

uint16_t CZBinaryReadUInt16LE(const uint8_t * bytes, size_t * readSize) {
    const size_t size = sizeof(uint16_t);
    uint16_t value = 0;
    for (size_t i = 0; i < size; i += 1) {
        value |= (uint16_t)bytes[i] << (8 * i);
    }
    if (readSize) {
        *readSize += size;
    }
    return value;
}

uint32_t CZBinaryReadUInt32LE(const uint8_t * bytes, size_t * readSize) {
    const size_t size = sizeof(uint32_t);
    uint32_t value = 0;
    for (size_t i = 0; i < size; i += 1) {
        value |= (uint32_t)bytes[i] << (8 * i);
    }
    if (readSize) {
        *readSize += size;
    }
    return value;
}

uint64_t CZBinaryReadUInt64LE(const uint8_t * bytes, size_t * readSize) {
    const size_t size = sizeof(uint64_t);
    uint64_t value = 0;
    for (size_t i = 0; i < size; i += 1) {
        value |= (uint64_t)bytes[i] << (8 * i);
    }
    if (readSize) {
        *readSize += size;
    }
    return value;
}

size_t CZBinaryWriteUInt8(uint8_t * bytes, uint8_t value) {
    const size_t size = sizeof(uint8_t);
    bytes[0] = value;
    return size;
}

size_t CZBinaryWriteUInt16LE(uint8_t * bytes, uint16_t value) {
    const size_t size = sizeof(uint16_t);
    for (size_t i = 0; i < size; i += 1) {
        bytes[i] = (value >> (8 * i)) & 0xff;
    }
    return size;
}

size_t CZBinaryWriteUInt32LE(uint8_t * bytes, uint32_t value) {
    const size_t size = sizeof(uint32_t);
    for (size_t i = 0; i < size; i += 1) {
        bytes[i] = (value >> (8 * i)) & 0xff;
    }
    return size;
}

size_t CZBinaryWriteUInt64LE(uint8_t * bytes, uint64_t value) {
    const size_t size = sizeof(uint64_t);
    for (size_t i = 0; i < size; i += 1) {
        bytes[i] = (value >> (8 * i)) & 0xff;
    }
    return size;
}
