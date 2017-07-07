//
//  CZZip_internal.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/06/05.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZZip_internal_h
#define CZZip_internal_h

#include "CZZip.h"
#include "CZStream.h"

CZStreamRef _Nonnull CZZipGetStream(const CZZipRef _Nonnull obj);
CZStreamRef _Nonnull CZZipGetTempStream(const CZZipRef _Nonnull obj);

#endif /* CZZip_internal_h */
