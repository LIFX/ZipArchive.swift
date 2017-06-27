//
//  CZDecompressDeflate.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/18.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZDecompressDeflate_h
#define CZDecompressDeflate_h

#include "CZDecompress.h"

#include <zlib.h>

CZDecompressRef _Nonnull CZDecompressDeflateCreate(CZStreamRef _Nonnull stream, swift_int_t bufferSize);

#endif /* CZDecompressDeflate_h */
