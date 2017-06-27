//
//  CZStreamFile.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/15.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZStreamFile_h
#define CZStreamFile_h

#include "CZUtility.h"
#include "CZStream.h"

typedef enum {
    CZStreamFileModeRead,
    CZStreamFileModeWrite
} CZStreamFileMode;

CZStreamRef _Nullable CZStreamCreateWithFileAtPath(const char * _Nonnull path, CZStreamFileMode mode);

#endif
