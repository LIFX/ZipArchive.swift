//
//  CZZipEntry_internal.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/16.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZZipEntry_internal_h
#define CZZipEntry_internal_h

#include "CZZip.h"
#include "CZZipEntry.h"
#include "CZCompress.h"

CZZipEntryRef _Nonnull CZZipEntryCreate(CZZipRef _Nonnull zip, const char * _Nonnull entryName, CZDateTime time, swift_int_t method, swift_int_t level, CZCompressFactoryFunc _Nonnull compressFactory, CZCryptoRef _Nullable crypto, uint32_t crc32);

#endif /* CZZipEntry_internal_h */
