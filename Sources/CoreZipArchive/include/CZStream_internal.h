//
//  CZStream_internal.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/06/17.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZStream_internal_h
#define CZStream_internal_h

typedef enum {
    CZStreamSubTypeOther = 0,
    CZStreamSubTypeFile = 1,
    CZStreamSubTypeInMemory = 2,
    CZStreamSubTypeBuffer = 3
} CZStreamSubType;

void CZStreamSetSubType(CZStreamRef obj, CZStreamSubType t);

CZStreamSubType CZStreamGetSubType(CZStreamRef obj);

#endif /* CZStream_internal_h */
