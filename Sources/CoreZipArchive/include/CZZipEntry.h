//
//  CZZipEntry.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/16.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZZipEntry_h
#define CZZipEntry_h

#include "CZUtility.h"
#include "CZCompress.h"
#include "CZTypes.h"

typedef struct CZZipEntry * CZZipEntryRef;

void CZZipEntryRelease(CZZipEntryRef _Nonnull obj);

swift_int_t CZZipEntryWrite(CZZipEntryRef _Nonnull obj, const uint8_t * _Nonnull buffer, swift_int_t length);

uint32_t CZZipGetCRC32(CZZipEntryRef _Nonnull obj);

bool CZZipEntryClose(CZZipEntryRef _Nonnull obj);

#endif /* CZZipEntry_h */
