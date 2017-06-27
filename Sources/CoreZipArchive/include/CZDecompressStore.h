//
//  CZDecompressStore.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/18.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZDecompressStore_h
#define CZDecompressStore_h

#include "CZDecompress.h"

CZDecompressRef _Nonnull CZDecompressStoreCreate(CZStreamRef _Nonnull stream, swift_int_t fileSize);

#endif /* CZDecompressStore_h */
