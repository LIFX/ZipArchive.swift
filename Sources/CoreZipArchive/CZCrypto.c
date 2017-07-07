//
//  CZCrypto.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/06/13.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZCrypto.h"

#include <time.h>
#include <zlib.h>

struct CZCrypto {

    const z_crc_t * crc32Table;
    uint32_t keys[3];
    
};

// MARK: - private

bool _CZCryptoIsNeedToInitialize = true;

static inline uint32_t _CZCryptoCRC32(const z_crc_t * table, uint32_t old, uint32_t c) {
    uint8_t index = (old ^ c) & 0xff;
    return (uint32_t)table[index] ^ (old >> 8);
}

static inline void _CZCryptoUpdateKeys(CZCryptoRef obj, uint8_t c) {
    obj->keys[0] = _CZCryptoCRC32(obj->crc32Table, obj->keys[0], c);
    obj->keys[1] = obj->keys[1] + (obj->keys[0] & 0xff);
    obj->keys[1] = obj->keys[1] * 134775813 + 1;
    obj->keys[2] = _CZCryptoCRC32(obj->crc32Table, obj->keys[2], obj->keys[1] >> 24);
}

static inline uint8_t _CZCryptoDecryptByte(CZCryptoRef obj) {
    uint16_t temp = (obj->keys[2] & 0xffff) | 2;
    return ((temp * (temp ^ 1)) >> 8) & 0xff;
}

// MARK: - public

CZCryptoRef CZCryptoCreate(const char * password) {
    CZCryptoRef obj = calloc(1, sizeof(struct CZCrypto));
#if HAS_DEFLATE
    // FIXME:
    obj->crc32Table = get_crc_table();
#endif
    obj->keys[0] = 305419896;
    obj->keys[1] = 591751049;
    obj->keys[2] = 878082192;
    
    while (*password) {
        _CZCryptoUpdateKeys(obj, *password);
        password += 1;
    }

    return obj;
}

void CZCryptoRelease(CZCryptoRef obj) {
    free(obj);
}

uint8_t CZCryptoDecode(CZCryptoRef obj, uint8_t c) {
    uint8_t temp = c ^ _CZCryptoDecryptByte(obj);
    _CZCryptoUpdateKeys(obj, temp);
    return temp;
}

uint8_t CZCryptoEncode(CZCryptoRef obj, uint8_t c) {
    uint8_t temp = _CZCryptoDecryptByte(obj);
    _CZCryptoUpdateKeys(obj, c);
    return temp ^ c;
}

void CZCryptoMakeHeader(CZCryptoRef obj, uint32_t crc32, uint8_t header[CZCryptoHeaderSize]) {
    if (_CZCryptoIsNeedToInitialize) {
        srand((uint32_t)time(NULL));
        _CZCryptoIsNeedToInitialize = false;
    }
    for (int32_t i = 0; i < CZCryptoHeaderSize - 1; i += 1) {
        header[i] = CZCryptoEncode(obj, (uint8_t)(rand() & 0xff));
    }
    if (crc32 == 0) {
        header[CZCryptoHeaderSize - 1] = CZCryptoEncode(obj, (uint8_t)(rand() & 0xff));
    } else {
        header[CZCryptoHeaderSize - 1] = CZCryptoEncode(obj, (uint8_t)((crc32 >> 24) & 0xff));
    }
}
