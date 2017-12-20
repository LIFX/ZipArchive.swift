//
//  CZCompressDeflate.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/18.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZCompressDeflate_h
#define CZCompressDeflate_h

#include "CZCompress.h"

#include <zlib.h>

CZCompressRef _Nullable CZCompressDeflateCreate(CZStreamRef _Nonnull stream, swift_int_t level, swift_int_t bufferSize);

#endif /* CZCompressDeflate_h */
