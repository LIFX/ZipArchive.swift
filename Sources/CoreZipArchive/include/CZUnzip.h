//
//  CZUnzip.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/15.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZUnzip_h
#define CZUnzip_h

#include "CZUtility.h"
#include "CZStream.h"
#include "CZTypes.h"

typedef struct CZUnzip * CZUnzipRef;

#include "CZUnzipEntry.h"

CZUnzipRef _Nullable CZUnzipCreate(CZStreamRef _Nonnull stream);

void CZUnzipRelease(CZUnzipRef _Nonnull unzip);

const CZEndOfCentralDirectoryRecordRef _Nullable CZUnzipGetEndOfCentralDirectoryRecord(const CZUnzipRef _Nonnull obj);

// MARK: - Iterator

void CZUnzipResetIterator(CZUnzipRef _Nonnull unzip);
bool CZUnzipMoveToNextEntry(CZUnzipRef _Nonnull unzip);
bool CZUnzipMoveToSpecifiedNameEntry(CZUnzipRef _Nonnull unzip, const char * _Nonnull fileName);

const CZEntryHeaderRef _Nullable CZUnzipGetCurrentGlobalHeader(const CZUnzipRef _Nonnull obj);
CZUnzipEntryRef _Nullable CZUnzipGetCurrentEntry(CZUnzipRef _Nonnull unzip);

#endif /* CZUnzip_h */
