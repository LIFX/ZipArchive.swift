//
//  CZZip.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/15.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZZip_h
#define CZZip_h

#include "CZUtility.h"
#include "CZStream.h"
#include "CZDateTime.h"

typedef struct CZZip * CZZipRef;

#include "CZZipEntry.h"

CZZipRef _Nonnull CZZipCreate(CZStreamRef _Nonnull stream);

CZZipRef _Nonnull CZZipCreateWithTemporaryStream(CZStreamRef _Nonnull stream, CZStreamRef _Nonnull tempStream, bool closeTempStreamWhenRelease);

void CZZipRelease(CZZipRef _Nonnull obj);

CZZipEntryRef _Nonnull CZZipAppendEntry(CZZipRef _Nonnull obj, const char * _Nonnull entryName, swift_int_t method, CZDateTime time, swift_int_t level, CZCompressFactoryFunc _Nullable compressFactory, const char * _Nullable password, uint32_t crc32);

void CZZipClose(CZZipRef _Nonnull obj);

swift_int_t CZZipGetEntryCount(CZZipRef _Nonnull obj);

#endif /* CZZip_h */

