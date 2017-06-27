//
//  CZStreamInMemory.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/17.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZStreamInMemory_h
#define CZStreamInMemory_h

#include "CZUtility.h"
#include "CZStream.h"

CZStreamRef _Nonnull CZStreamCreateInMemory(void);

swift_int_t CZStreamGetMemoryBufferLength(CZStreamRef _Nonnull obj);
const void * _Nullable CZStreamGetMemoryBuffer(CZStreamRef _Nonnull obj);

#endif
