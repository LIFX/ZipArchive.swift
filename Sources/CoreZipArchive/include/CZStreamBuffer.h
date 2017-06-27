//
//  CZStreamBuffer.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/06/17.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZStreamBuffer_h
#define CZStreamBuffer_h

#include "CZUtility.h"
#include "CZStream.h"

CZStreamRef _Nonnull CZStreamCreateWithBuffer(void * _Nonnull buffer, size_t capacity);

#endif /* CZStreamBuffer_h */
