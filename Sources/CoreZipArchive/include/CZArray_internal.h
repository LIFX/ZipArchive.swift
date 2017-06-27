//
//  CZArray_internal.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/16.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZArray_h
#define CZArray_h

#include "CZUtility.h"

typedef struct CZArray * CZArrayRef;

CZArrayRef _Nonnull CZArrayCreate(void);

void CZArrayRelease(CZArrayRef _Nonnull obj);

swift_int_t CZArrayPushBack(CZArrayRef _Nonnull obj, void * _Nonnull element);

void * _Nullable CZArrayPopBack(CZArrayRef _Nonnull obj);

void * _Nullable CZArrayGetElementAtIndex(const CZArrayRef _Nonnull obj, swift_int_t index);

swift_int_t CZArrayGetCount(const CZArrayRef _Nonnull obj);

#endif /* CZArray_h */
