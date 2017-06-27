//
//  CZDateTime.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/06/18.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZDateTime.h"

swift_int_t CZDateTimeGetZipDate(CZDateTime t) {
    swift_int_t date = 0;
    swift_int_t year = 0;
    if (t.year > 1980) {
        year = t.year - 1980;
    }
    date += year << 9;
    date += t.month << 5;
    date += t.day;
    return date;
}

swift_int_t CZDateTimeGetZipTime(CZDateTime t) {
    swift_int_t time = 0;
    time += t.hour << 11;
    time += t.minute << 5;
    time += t.second / 2;
    return time;
}

CZDateTime CZDateTimeGetDateTime(swift_int_t date, swift_int_t time) {
    CZDateTime t = {0};
    
    t.year      = ((date & 0b1111111000000000) >> 9) + 1980;
    t.month     = ((date & 0b0000000111100000) >> 5);
    t.day       = ( date & 0b0000000000011111);
    
    t.hour      = ((time & 0b1111100000000000) >> 11);
    t.minute    = ((time & 0b0000011111100000) >> 5);
    t.second    = ( time & 0b0000000000011111) * 2;
    
    return t;
}
