//
//  CZTypes.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/17.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZTypes.h"

#include "CZArray_internal.h"
#include "CZData_internal.h"

static inline void CopyString(const CZDataRef data, char * buffer, swift_int_t bufferSize) {
    if (bufferSize == 0) {
        return;
    }
    if (data == NULL) {
        buffer[0] = '\0';
        return;
    }
    swift_int_t count = CZDataGetLength(data);
    if (count > bufferSize) {
        buffer[0] = '\0';
        return;
    }
    memcpy(buffer, CZDataGetBytes(data), count);
    if (bufferSize > count) {
        buffer[count] = '\0';
    }
}

// -----------------------------------------------------------------------------
// MARK: - CZExtraField

struct CZExtraField {
    
    uint16_t headerID;
    CZDataRef data;
    
};

// private
CZExtraFieldRef CZExtraFieldCreate(uint16_t id, const uint8_t * data, swift_int_t length) {
    CZExtraFieldRef obj = calloc(1, sizeof(struct CZExtraField));
    
    obj->headerID = id;
    
    CZDataRef d = CZDataCreate();
    CZDataAppendWithBytes(d, data, length);
    obj->data = d;
    
    return obj;
}

// private
void CZExtraFieldRelease(CZExtraFieldRef obj) {
    if (obj->data) {
        CZDataRelease(obj->data);
        obj->data = NULL;
    }
    free(obj);
}

uint16_t CZExtraFieldGetHeaderID(const CZExtraFieldRef obj) {
    return obj->headerID;
}

swift_int_t CZExtraFieldGetDataLength(const CZExtraFieldRef obj) {
    return CZDataGetLength(obj->data);
}

const uint8_t * CZExtraFieldGetData(const CZExtraFieldRef obj) {
    return CZDataGetBytes(obj->data);
}

// -----------------------------------------------------------------------------
// MARK: - CZEntryHeader

struct CZEntryHeader {
    
    CZEntryHeaderInfo info;
    CZDataRef fileName;
    CZArrayRef extraFields; // Array of CZExtraField
    CZDataRef fileComment;
    
};

CZEntryHeaderRef CZEntryHeaderCreate(void) {
    CZEntryHeaderRef obj = calloc(1, sizeof(struct CZEntryHeader));
    return obj;
}

void CZEntryHeaderRelease(CZEntryHeaderRef obj) {
    if (obj->fileName) {
        CZDataRelease(obj->fileName);
        obj->fileName = NULL;
    }
    if (obj->extraFields) {
        void * element;
        while ((element = CZArrayPopBack(obj->extraFields)) != NULL) {
            CZExtraFieldRelease(element);
        }
        CZArrayRelease(obj->extraFields);
        obj->extraFields = NULL;
    }
    if (obj->fileComment) {
        CZDataRelease(obj->fileComment);
        obj->fileComment = NULL;
    }
    free(obj);
}

CZEntryHeaderInfo CZEntryHeaderGetInfo(const CZEntryHeaderRef obj) {
    return obj->info;
}

void CZEntryHeaderSetInfo(CZEntryHeaderRef obj, CZEntryHeaderInfo info) {
    obj->info = info;
}

swift_int_t CZEntryHeaderGetFileNameLength(const CZEntryHeaderRef _Nonnull obj) {
    if (obj->fileName == NULL) {
        return 0;
    }
    return CZDataGetLength(obj->fileName);
}

void CZEntryHeaderGetFileName(const CZEntryHeaderRef obj, char * buffer, swift_int_t bufferSize) {
    CopyString(obj->fileName, buffer, bufferSize);
}

void CZEntryHeaderSetFileName(CZEntryHeaderRef obj, const char * str, swift_int_t length) {
    if (obj->fileName) {
        CZDataSetLength(obj->fileName, 0);
    } else {
        obj->fileName = CZDataCreate();
    }

    CZDataAppendWithBytes(obj->fileName, (const void *)str, length);
}

swift_int_t CZEntryHeaderGetExtraFieldCount(const CZEntryHeaderRef obj) {
    if (obj->extraFields == NULL) {
        return 0;
    }
    return CZArrayGetCount(obj->extraFields);
}

const CZExtraFieldRef CZEntryHeaderGetExtraFieldAtIndex(const CZEntryHeaderRef obj, swift_int_t index) {
    if (obj->extraFields == NULL) {
        return 0;
    }
    return CZArrayGetElementAtIndex(obj->extraFields, index);
}

swift_int_t CZEntryHeaderAppendExtraField(CZEntryHeaderRef obj, uint16_t id, const uint8_t * data, swift_int_t length) {
    if (obj->extraFields == NULL) {
        obj->extraFields = CZArrayCreate();
    }
    
    CZExtraFieldRef extraField = CZExtraFieldCreate(id, data, length);
    
    return CZArrayPushBack(obj->extraFields, extraField);
}

swift_int_t CZEntryHeaderGetFileCommentLength(const CZEntryHeaderRef _Nonnull obj) {
    if (obj->fileComment == NULL) {
        return 0;
    }
    return CZDataGetLength(obj->fileComment);
}

void CZEntryHeaderGetFileComment(const CZEntryHeaderRef obj, char * buffer, swift_int_t bufferSize) {
    CopyString(obj->fileComment, buffer, bufferSize);
}

void CZEntryHeaderSetFileComment(CZEntryHeaderRef obj, const char * str, swift_int_t length) {
    if (obj->fileComment) {
        CZDataSetLength(obj->fileComment, 0);
    } else {
        obj->fileComment = CZDataCreate();
    }
    
    CZDataAppendWithBytes(obj->fileComment, (const void *)str, length);
}

// -----------------------------------------------------------------------------
// MARK: - CZEndOfCentralDirectory

struct CZEndOfCentralDirectoryRecord {
    
    CZEndOfCentralDirectoryRecordInfo info;
    CZDataRef fileComment;

};

CZEndOfCentralDirectoryRecordRef CZEndOfCentralDirectoryRecordCreate(void) {
    CZEndOfCentralDirectoryRecordRef obj = calloc(1, sizeof(struct CZEndOfCentralDirectoryRecord));
    return obj;
}

void CZEndOfCentralDirectoryRecordRelease(CZEndOfCentralDirectoryRecordRef obj) {
    if (obj->fileComment) {
        CZDataRelease(obj->fileComment);
        obj->fileComment = NULL;
    }
    free(obj);
}

CZEndOfCentralDirectoryRecordInfo CZEndOfCentralDirectoryRecordGetInfo(const CZEndOfCentralDirectoryRecordRef obj) {
    return obj->info;
}

void CZEndOfCentralDirectoryRecordSetInfo(CZEndOfCentralDirectoryRecordRef obj, CZEndOfCentralDirectoryRecordInfo info) {
    obj->info = info;
}

swift_int_t CZEndOfCentralDirectoryRecordGetFileCommnetLength(const CZEndOfCentralDirectoryRecordRef obj) {
    if (obj->fileComment == NULL) {
        return 0;
    }
    return CZDataGetLength(obj->fileComment);
}

void CZEndOfCentralDirectoryRecordGetFileCommnet(const CZEndOfCentralDirectoryRecordRef obj, char * buffer, swift_int_t bufferSize) {
    CopyString(obj->fileComment, buffer, bufferSize);
}

void CZEndOfCentralDirectoryRecordSetFileCommnet(CZEndOfCentralDirectoryRecordRef obj, const char * str, swift_int_t length) {
    if (obj->fileComment) {
        CZDataSetLength(obj->fileComment, 0);
    } else {
        obj->fileComment = CZDataCreate();
    }

    CZDataAppendWithBytes(obj->fileComment, (const void *)str, length);
}
