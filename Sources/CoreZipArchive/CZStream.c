//
//  CZStream.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/15.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZStream.h"

#include "CZStream_internal.h"

struct CZStream {
    
    // FIXME: isClosed を設けて、二重クローズを防ぐ
    // その前に、そう言う使われ方をされている箇所を治す
    // Zipのstreamがdeinit で解放される前にcloseしている問題
    
    CZStreamSubType subType;
    
    CZStreamFunctions functions;
    void * extraData;
    
    CZCryptoRef crypto;
    
};

// internal
void CZStreamSetSubType(CZStreamRef obj, CZStreamSubType t) {
    obj->subType = t;
}

// internal
CZStreamSubType CZStreamGetSubType(CZStreamRef obj) {
    return obj->subType;
}

CZStreamRef CZStreamCreate(CZStreamFunctions functions, void * extraData) {
    CZStreamRef obj = calloc(1, sizeof(struct CZStream));
    obj->functions = functions;
    obj->extraData = extraData;
    if (functions.open) {
        functions.open(extraData);
    }
    return obj;
}

void CZStreamRelease(CZStreamRef obj) {
    free(obj);
}

swift_int_t CZStreamClose(CZStreamRef obj) {
    if (obj->functions.close) {
        return obj->functions.close(obj->extraData);
    } else {
        return 0;
    }
}

size_t CZStreamRead(CZStreamRef obj, uint8_t * buffer, size_t count) {
    const size_t len = obj->functions.read(obj->extraData, buffer, count);
    if (obj->crypto) {
        for (size_t i = 0; i < len; i += 1) {
            buffer[i] = CZCryptoDecode(obj->crypto, buffer[i]);
        }
    }
    return len;
}

size_t CZStreamWrite(CZStreamRef obj, const uint8_t * buffer, size_t count) {
    if (obj->crypto) {
        uint8_t cryptoBuffer[count];
        for (size_t i = 0; i < count; i += 1) {
            cryptoBuffer[i] = CZCryptoEncode(obj->crypto, buffer[i]);
        }
        return obj->functions.write(obj->extraData, cryptoBuffer, count);
    } else {
        return obj->functions.write(obj->extraData, buffer, count);
    }
}

swift_int_t CZStreamSeek(CZStreamRef obj, int64_t offset, CZStreamSeekOrigin origin) {
    return obj->functions.seek(obj->extraData, offset, origin);
}

int64_t CZStreamTell(CZStreamRef obj) {
    return obj->functions.tell(obj->extraData);
}

swift_int_t CZStreamGetErrorCode(CZStreamRef obj) {
    if (obj->functions.error) {
        return obj->functions.error(obj->extraData);
    } else {
        return 0;
    }
}

void * CZStreamGetExtraData(const CZStreamRef obj) {
    return obj->extraData;
}

void CZStreamAttachCrypto(CZStreamRef obj, CZCryptoRef crypto) {
    if (obj->crypto) {
        // [warning]
    }
    obj->crypto = crypto;
}

CZCryptoRef CZStreamDetachCrypto(CZStreamRef obj) {
    CZCryptoRef crypto = obj->crypto;
    obj->crypto = NULL;
    return crypto;
}
