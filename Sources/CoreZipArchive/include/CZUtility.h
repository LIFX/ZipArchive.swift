//
//  CZUtility.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/15.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZUtility_h
#define CZUtility_h

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define swift_int_t long
#define swift_uint_t unsigned long

#define SWIFT_NAME(x) __attribute__((swift_name(#x)))
#define SWIFT_UNAVAILABLE __attribute__((availability(swift,unavailable)))

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

extern swift_int_t CZDefaultBufferSize;

#endif /* CZUtility_h */
