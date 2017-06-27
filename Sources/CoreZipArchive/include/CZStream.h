//
//  CZStream.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/15.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZStream_h
#define CZStream_h

#include "CZUtility.h"
#include "CZCrypto.h"

typedef struct CZStream * CZStreamRef;

typedef enum {
    
    CZStreamSeekOriginBegin = 0,
    CZStreamSeekOriginCurrent,
    CZStreamSeekOriginEnd
    
} CZStreamSeekOrigin;

typedef void (*CZStreamOpenFunc)(void * _Nullable opaque);

typedef swift_int_t (*CZStreamCloseFunc)(void * _Nullable opaque);

typedef size_t (*CZStreamReadFunc)(void * _Nullable opaque, uint8_t * _Nonnull buffer, size_t count);

typedef size_t (*CZStreamWriteFunc)(void * _Nullable opaque, const uint8_t * _Nonnull buffer, size_t count);

typedef swift_int_t (*CZStreamSeekFunc)(void * _Nullable opaque, int64_t offset, CZStreamSeekOrigin origin);

typedef int64_t (*CZStreamTellFunc)(void * _Nullable opaque);

typedef swift_int_t (*CZStreamErrorFunc)(void * _Nullable opaque);

typedef struct {
    
    CZStreamOpenFunc _Nullable open;
    CZStreamCloseFunc _Nullable close;
    
    CZStreamReadFunc _Nonnull read;
    CZStreamWriteFunc _Nonnull write;
    
    CZStreamSeekFunc _Nonnull seek;
    CZStreamTellFunc _Nonnull tell;
    
    CZStreamErrorFunc _Nullable error;
    
} CZStreamFunctions;

CZStreamRef _Nonnull CZStreamCreate(CZStreamFunctions functions, void * _Nullable extraData);

swift_int_t CZStreamRelease(CZStreamRef _Nonnull obj);

size_t CZStreamRead(CZStreamRef _Nonnull obj, uint8_t * _Nonnull buffer, size_t count);

size_t CZStreamWrite(CZStreamRef _Nonnull obj, const uint8_t * _Nonnull buffer, size_t count);

swift_int_t CZStreamSeek(CZStreamRef _Nonnull obj, int64_t offset, CZStreamSeekOrigin origin);

int64_t CZStreamTell(CZStreamRef _Nonnull obj);

swift_int_t CZStreamGetErrorCode(CZStreamRef _Nonnull obj);

void * _Nullable CZStreamGetExtraData(const CZStreamRef _Nonnull obj);

void CZStreamAttachCrypto(CZStreamRef _Nonnull obj, CZCryptoRef _Nonnull crypto);

CZCryptoRef _Nullable CZStreamDetachCrypto(CZStreamRef _Nonnull obj);

#endif
