//
//  CZCompress.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/16.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZCompress_h
#define CZCompress_h

#include "CZUtility.h"
#include "CZStream.h"
#include "CZTypes.h"

typedef struct CZCompress * CZCompressRef;

typedef swift_int_t (*CZCompressWriteFunc)(CZStreamRef _Nonnull stream, const uint8_t * _Nonnull buffer, swift_int_t length, void * _Nullable opaque);

typedef void (*CZCompressReleaseFunc)(void * _Nullable opaque);

typedef swift_int_t (*CZCompressGetOriginalSizeFunc)(void * _Nullable opaque);

typedef swift_int_t (*CZCompressGetCompressedSizeFunc)(void * _Nullable opaque);

typedef CZCompressRef _Nullable (*CZCompressFactoryFunc)(swift_int_t method, swift_int_t level, CZStreamRef _Nonnull stream);

#include "CZCompressStore.h"
#include "CZCompressDeflate.h"

CZCompressRef _Nonnull CZCompressCreate(CZStreamRef _Nonnull stream, CZCompressWriteFunc _Nonnull write, CZCompressReleaseFunc _Nullable release, CZCompressGetOriginalSizeFunc _Nonnull getOriginalSize, CZCompressGetCompressedSizeFunc _Nonnull getCompressedSize);

void CZCompressRelease(CZCompressRef _Nonnull obj);

void CZCompressSetMethod(CZCompressRef _Nonnull obj, swift_int_t method);
swift_int_t CZCompressGetMethod(CZCompressRef _Nonnull obj);

CZStreamRef _Nonnull CZCompressGetStream(const CZCompressRef _Nonnull obj);

void * _Nullable CZCompressGetExtraData(const CZCompressRef _Nonnull obj);

void CZCompressSetExtraData(CZCompressRef _Nonnull obj, void * _Nullable opaque);

//uint16_t CZCompressGetMethod(CZCompressRef _Nonnull obj);

swift_int_t CZCompressWrite(CZCompressRef _Nonnull obj, const uint8_t * _Nonnull buffer, swift_int_t length);

swift_int_t CZCompressGetOriginalSize(CZCompressRef _Nonnull obj);

swift_int_t CZCompressGetCompressedSize(CZCompressRef _Nonnull obj);

uint32_t CZCompressGetCRC32(CZCompressRef _Nonnull obj);

CZCompressRef _Nullable CZCompressDefaultFactory(swift_int_t method, swift_int_t level, CZStreamRef _Nonnull stream);

#endif
