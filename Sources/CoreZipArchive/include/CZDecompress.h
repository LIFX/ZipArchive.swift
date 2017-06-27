//
//  CZDecompress.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/15.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZDecompress_h
#define CZDecompress_h

#include "CZUtility.h"
#include "CZStream.h"
#include "CZTypes.h"

typedef struct CZDecompress * CZDecompressRef;

typedef swift_int_t (*CZDecompressReadFunc)(CZStreamRef _Nonnull stream, uint8_t * _Nonnull buffer, swift_int_t length, void * _Nullable opaque);

typedef void (*CZDecompressReleaseFunc)(void * _Nullable opaque);

typedef CZDecompressRef _Nullable (*CZDecompressFactoryFunc)(CZEntryHeaderInfo info, CZStreamRef _Nonnull stream);

#include "CZDecompressStore.h"
#include "CZDecompressDeflate.h"

CZDecompressRef _Nonnull CZDecompressCreate(CZStreamRef _Nonnull stream, CZDecompressReadFunc _Nonnull read, CZDecompressReleaseFunc _Nullable release);

void CZDecompressRelease(CZDecompressRef _Nonnull obj);

void CZDecompressSetMethod(CZDecompressRef _Nonnull obj, swift_int_t method);
    
swift_int_t CZDecompressGetMethod(CZDecompressRef _Nonnull obj);
    
CZStreamRef _Nonnull CZDecompressGetStream(const CZDecompressRef _Nonnull obj);

void * _Nullable CZDecompressGetExtraData(const CZDecompressRef _Nonnull obj);

void CZDecompressSetExtraData(CZDecompressRef _Nonnull obj, void * _Nullable opaque);

swift_int_t CZDecompressRead(CZDecompressRef _Nonnull obj, uint8_t * _Nonnull buffer, swift_int_t length);

uint32_t CZDecompressGetCRC32(CZDecompressRef _Nonnull obj);

CZDecompressRef _Nullable CZDecompressDefaultFactory(CZEntryHeaderInfo info, CZStreamRef _Nonnull stream);

#endif
