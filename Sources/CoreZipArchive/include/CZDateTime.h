//
//  CZDateTime.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/06/18.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZDateTime_h
#define CZDateTime_h

#include "CZUtility.h"

typedef struct {
    
    swift_int_t year;
    swift_int_t month;
    swift_int_t day;
    
    swift_int_t hour;
    swift_int_t minute;
    swift_int_t second;
    
} CZDateTime SWIFT_NAME(ZipArchiveDateTime);

swift_int_t CZDateTimeGetZipDate(CZDateTime t) SWIFT_NAME(getter:ZipArchiveDateTime.zipDate(self:));
swift_int_t CZDateTimeGetZipTime(CZDateTime t) SWIFT_NAME(getter:ZipArchiveDateTime.zipTime(self:));
CZDateTime CZDateTimeGetDateTime(swift_int_t date, swift_int_t time) SWIFT_NAME(ZipArchiveDateTime.init(date:time:));

#endif /* CZDateTime_h */
