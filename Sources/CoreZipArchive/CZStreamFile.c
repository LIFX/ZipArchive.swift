//
//  CZStreamFile.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/15.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZStreamFile.h"

#include "CZStream_internal.h"

#include <stdio.h>

// MARK: - private

swift_int_t _CZStreamFileClose(void * opaque) {
    FILE * fp = opaque;
    return fclose(fp);
}

size_t _CZStreamFileRead(void * opaque, uint8_t * buffer, size_t count) {
    FILE * fp = opaque;
    return fread(buffer, sizeof(uint8_t), count, fp);
}

size_t _CZStreamFileWrite(void * opaque, const uint8_t * buffer, size_t count) {
    FILE * fp = opaque;
    return fwrite(buffer, sizeof(uint8_t), count, fp);
}

swift_int_t _CZStreamFileSeek(void * opaque, int64_t offset, CZStreamSeekOrigin origin) {
    FILE * fp = opaque;
    int32_t flag = -1;
    switch (origin) {
        case CZStreamSeekOriginBegin:
            flag = SEEK_SET;
            break;
        case CZStreamSeekOriginCurrent:
            flag = SEEK_CUR;
            break;
        case CZStreamSeekOriginEnd:
            flag = SEEK_END;
            break;
        default:
            return -1;
    }
    return fseek(fp, offset, flag);
}

int64_t _CZStreamFileTell(void * opaque) {
    FILE * fp = opaque;
    return ftell(fp);
}

swift_int_t _CZStreamFileGetErrorCode(void * opaque) {
    FILE * fp = opaque;
    return ferror(fp);
}

// MARK: - public

CZStreamRef CZStreamCreateWithFileAtPath(const char * path, CZStreamFileMode mode) {
    FILE * fp = NULL;
    switch (mode) {
        case CZStreamFileModeRead:
            fp = fopen(path, "rb");
            break;
        case CZStreamFileModeWrite:
            fp = fopen(path, "wb+");
            break;
    }
    if (fp == NULL) {
        return NULL;
    }

    CZStreamFunctions funcs = {0};
    funcs.open = NULL;
    funcs.close = _CZStreamFileClose;
    funcs.read = _CZStreamFileRead;
    funcs.write = _CZStreamFileWrite;
    funcs.seek = _CZStreamFileSeek;
    funcs.tell = _CZStreamFileTell;
    funcs.error = _CZStreamFileGetErrorCode;
    
    CZStreamRef stream = CZStreamCreate(funcs, fp);
    CZStreamSetSubType(stream, CZStreamSubTypeFile);

    return stream;
}
