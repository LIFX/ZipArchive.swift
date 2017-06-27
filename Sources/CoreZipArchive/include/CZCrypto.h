//
//  CZCrypto.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/06/13.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZCrypto_h
#define CZCrypto_h

#include "CZUtility.h"

#define CZCryptoHeaderSize 12L

typedef struct CZCrypto * CZCryptoRef;

CZCryptoRef _Nonnull CZCryptoCreate(const char * _Nonnull password);

void CZCryptoRelease(CZCryptoRef _Nonnull obj);

uint8_t CZCryptoDecode(CZCryptoRef _Nonnull obj, uint8_t c);

uint8_t CZCryptoEncode(CZCryptoRef _Nonnull obj, uint8_t c);

void CZCryptoMakeHeader(CZCryptoRef _Nonnull obj, uint32_t crc32, uint8_t header[_Nonnull CZCryptoHeaderSize]);

#endif /* CZCrypto_h */
