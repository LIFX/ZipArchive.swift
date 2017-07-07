//
//  CZBinaryUtility.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/06/18.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZBinaryUtility_h
#define CZBinaryUtility_h

#include "CZUtility.h"

uint8_t CZBinaryReadUInt8(const uint8_t * _Nonnull bytes, size_t * _Nullable readSize);
uint16_t CZBinaryReadUInt16LE(const uint8_t * _Nonnull bytes, size_t * _Nullable readSize);
uint32_t CZBinaryReadUInt32LE(const uint8_t * _Nonnull bytes, size_t * _Nullable readSize);
uint64_t CZBinaryReadUInt64LE(const uint8_t * _Nonnull bytes, size_t * _Nullable readSize);

size_t CZBinaryWriteUInt8(uint8_t * _Nonnull bytes, uint8_t value);
size_t CZBinaryWriteUInt16LE(uint8_t * _Nonnull bytes, uint16_t value);
size_t CZBinaryWriteUInt32LE(uint8_t * _Nonnull bytes, uint32_t value);
size_t CZBinaryWriteUInt64LE(uint8_t * _Nonnull bytes, uint64_t value);

#endif /* CZBinaryUtility_h */
