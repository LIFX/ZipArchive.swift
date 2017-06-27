//
//  CZUnzipEntry.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/15.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZUnzipEntry_h
#define CZUnzipEntry_h

#include "CZUtility.h"
#include "CZDecompress.h"
#include "CZTypes.h"

typedef struct CZUnzipEntry * CZUnzipEntryRef;

//#include "CZUnzip.h"

void CZUnzipEntryRelease(CZUnzipEntryRef _Nonnull obj);

void CZUnzipEntryOpen(CZUnzipEntryRef _Nonnull obj, const char * _Nullable password, CZDecompressFactoryFunc _Nullable decompressFactory);

void CZUnzipEntryClose(CZUnzipEntryRef _Nonnull obj);

swift_int_t CZUnzipEntryRead(CZUnzipEntryRef _Nonnull obj, uint8_t * _Nonnull buffer, swift_int_t length);

uint32_t CZUnzipGetCRC32(CZUnzipEntryRef _Nonnull obj);

const CZEntryHeaderRef _Nonnull CZUnzipEntryGetLocalHeader(const CZUnzipEntryRef _Nonnull obj);

//const CZEntryHeaderRef _Nonnull CZUnzipEntryGetGlobalHeader(const CZUnzipEntryRef _Nonnull obj);

#endif /* CZUnzipEntry_h */
