//
//  CZData.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/17.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZData_h
#define CZData_h

#include "CZUtility.h"

typedef struct CZData * CZDataRef;

CZDataRef _Nonnull CZDataCreate(void);

void CZDataRelease(CZDataRef _Nonnull obj);

void CZDataAppendWithBytes(CZDataRef _Nonnull obj, const uint8_t * _Nonnull bytes, swift_int_t length);

void CZDataAppendWithData(CZDataRef _Nonnull obj, const CZDataRef _Nonnull other);

const uint8_t * _Nonnull CZDataGetBytes(const CZDataRef _Nonnull obj);

uint8_t * _Nonnull CZDataGetMutableBytes(CZDataRef _Nonnull obj);

swift_int_t CZDataGetLength(const CZDataRef _Nonnull obj);

void CZDataSetLength(CZDataRef _Nonnull obj, swift_int_t newLength);

#endif /* CZData_h */
