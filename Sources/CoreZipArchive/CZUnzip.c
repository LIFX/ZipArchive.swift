//
//  CZUnzip.c
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/15.
//  Copyright © 2017 yaslab. All rights reserved.
//

#include "CZUnzip.h"

#include "CZTypes_internal.h"
#include "CZUnzipEntry_internal.h"
#include "CZBinaryUtility_internal.h"

struct CZUnzip {
    
    CZStreamRef stream;
    
    size_t currentHeaderOffset;
    size_t currentHeaderSize;
    CZEntryHeaderRef currentHeader;
    
    CZEndOfCentralDirectoryRecordRef endOfCentralDirectoryRecord;
    
};

// MARK: - private

static inline CZEndOfCentralDirectoryRecordRef _CZUnzipReadEndOfCentralDirectoryRecord(uint8_t * buffer) {
    size_t offset = 0;
    
    uint32_t signature = CZBinaryReadUInt32LE(buffer + offset, &offset);
    
    if (signature != CZEndOfCentralDirectorySignature) {
        // TODO:
        return NULL;
    }
    
    CZEndOfCentralDirectoryRecordRef record = CZEndOfCentralDirectoryRecordCreate();
    
    CZEndOfCentralDirectoryRecordInfo info = {0};
    info.numberOfThisDisk = CZBinaryReadUInt16LE(buffer + offset, &offset);
    info.numberOfTheDiskWithTheStartOfTheCentralDirectory = CZBinaryReadUInt16LE(buffer + offset, &offset);
    info.totalNumberOfEntriesInTheCentralDirectoryOnThisDisk = CZBinaryReadUInt16LE(buffer + offset, &offset);
    info.totalNumberOfEntriesInTheCentralDirectory = CZBinaryReadUInt16LE(buffer + offset, &offset);
    info.sizeOfTheCentralDirectory = CZBinaryReadUInt32LE(buffer + offset, &offset);
    info.offsetOfStartOfCentralDirectoryWithRespectToTheStartingDiskNumber = CZBinaryReadUInt32LE(buffer + offset, &offset);
    CZEndOfCentralDirectoryRecordSetInfo(record, info);
    
    uint16_t fileCommentLength = CZBinaryReadUInt16LE(buffer + offset, &offset);
    if (fileCommentLength > 0) {
        CZEndOfCentralDirectoryRecordSetFileCommnet(record, (const char *)(buffer + offset), fileCommentLength);
        offset += fileCommentLength;
    }
    
    return record;
}

static inline CZEndOfCentralDirectoryRecordRef _CZUnzipSearchEndOfCentralDirectoryRecord(CZStreamRef stream) {
    swift_int_t status = CZStreamSeek(stream, 0, CZStreamSeekOriginEnd);
    if (status < 0) {
        return NULL;
    }
    
    const size_t bufferSize = 1024 * 8;
    
    int64_t maxLength = CZStreamTell(stream);
    if (maxLength <= 4) {
        maxLength = 4;
    }
    if (maxLength > bufferSize) {
        maxLength = bufferSize;
    }
    
    status = CZStreamSeek(stream, -maxLength, CZStreamSeekOriginEnd);
    if (status < 0) {
        return NULL;
    }
    
    uint8_t buffer[bufferSize];
    
    size_t len = CZStreamRead(stream, buffer, maxLength);
    if (len != maxLength) {
        return NULL;
    }
    
    CZEndOfCentralDirectoryRecordRef record = NULL;
    
    for (int64_t i = (maxLength - 1); i >= 3; i -= 1) {
        if (buffer[i] == 0x06 && buffer[i - 1] == 0x05 && buffer[i - 2] == 0x4b && buffer[i - 3] == 0x50) {
            size_t offset = i - 3;
            //size_t length = maxLength - offset;
            record = _CZUnzipReadEndOfCentralDirectoryRecord(buffer + offset);
            break;
        }
    }
    
    return record;
}

static inline CZEntryHeaderRef _CZUnzipCreateCentralDirectoryHeader(CZStreamRef stream, size_t * size) {
    //        let data = NSData(bytesNoCopy: buffer, length: length, freeWhenDone: false)
    //
    //        byteSize = 0
    
    //let signature: UInt32 = BinaryUtility.deserialize(data, offset + byteSize, &byteSize)
    
    int64_t startSeekOffset = CZStreamTell(stream);
    
    size_t bufferSize = sizeof(CZEntryHeaderInfo) * 2;
    uint8_t * bytes = malloc(bufferSize);
    /*size_t readLen = */CZStreamRead(stream, bytes, bufferSize);
    
    size_t offset = 0;
    
    uint32_t signature = CZBinaryReadUInt32LE(bytes, &offset);
    
    if (signature != CZGlobalHeaderSignature) {
        // TODO:
        return NULL;
    }
    
    //        guard signature == CentralDirectoryHeader.signature else {
    //            // TODO: Error
    //            fatalError()
    //        }
    
    CZEntryHeaderInfo headerInfo = {0};
    
    //CentralDirectoryHeader *header = (CentralDirectoryHeader *)calloc(1, sizeof(CentralDirectoryHeader));
    headerInfo.versionMadeBy = CZBinaryReadUInt8(bytes + offset, &offset);
    headerInfo.systemInfo = CZBinaryReadUInt8(bytes + offset, &offset);
    headerInfo.versionNeededToExtract = CZBinaryReadUInt16LE(bytes + offset, &offset);
    headerInfo.flags = CZBinaryReadUInt16LE(bytes + offset, &offset);
    headerInfo.method = CZBinaryReadUInt16LE(bytes + offset, &offset);
    headerInfo.time = CZBinaryReadUInt16LE(bytes + offset, &offset);
    headerInfo.date = CZBinaryReadUInt16LE(bytes + offset, &offset);
    headerInfo.crc = CZBinaryReadUInt32LE(bytes + offset, &offset);
    headerInfo.compressedSize = CZBinaryReadUInt32LE(bytes + offset, &offset);
    headerInfo.size = CZBinaryReadUInt32LE(bytes + offset, &offset);
    uint16_t fileNameLength = CZBinaryReadUInt16LE(bytes + offset, &offset);
    uint16_t extraFieldLength = CZBinaryReadUInt16LE(bytes + offset, &offset);
    uint16_t fileCommentLength = CZBinaryReadUInt16LE(bytes + offset, &offset);
    headerInfo.diskNumberStart = CZBinaryReadUInt16LE(bytes + offset, &offset);
    headerInfo.internalFileAttributes = CZBinaryReadUInt16LE(bytes + offset, &offset);
    headerInfo.externalFileAttributes = CZBinaryReadUInt32LE(bytes + offset, &offset);
    headerInfo.localHeaderOffset = CZBinaryReadUInt32LE(bytes + offset, &offset);
    
    size_t newSize = fileNameLength + extraFieldLength + fileCommentLength;
    if (newSize > bufferSize) {
        uint8_t * newBytes = realloc(bytes, newSize);
        if (newBytes == NULL) {
            free(bytes);
        }
        bytes = newBytes;
        bufferSize = newSize;
    }
    
    // Header Size
    *size = offset + newSize;
    
    CZStreamSeek(stream, startSeekOffset + offset, CZStreamSeekOriginBegin);
    
    offset = 0;
    CZStreamRead(stream, bytes, newSize);
    
    
    CZEntryHeaderRef header = CZEntryHeaderCreate();
    CZEntryHeaderSetInfo(header, headerInfo);
    
    if (fileNameLength > 0) {
        CZEntryHeaderSetFileName(header, (const char *)(bytes + offset), fileNameLength);
        
        //CZDataRef fileName = CZDataCreate();
        //CZDataAppendWithBytes(fileName, bytes + offset, fileNameLength);
        //CZGlobalHeaderSetFileNameNoCopy(header, fileName);
        
        //        char *fileName = (char *)calloc(header->fileNameLength + 1, sizeof(char));
        //        memcpy(fileName, bytes + offset, header->fileNameLength);
        //        header->fileName = fileName;
        offset += fileNameLength;
    }
    
    
    
    //        let fileName = data.subdata(with: NSRange(location: offset + byteSize, length: Int(fileNameLength)))
    //        byteSize += fileName.count
    
    if (extraFieldLength > 0) {
        // TODO: Extra Field を作る
        //ExtraField * test;
        
        //header->extraField = CZUnzipCreateExtraFieldArray(bytes + offset, header->extraFieldLength);
        
        size_t o = 0;
        while (o < extraFieldLength) {
            if ((o + 4) > extraFieldLength) {
                // field length error!!
                break;
            }
            
            //CZExtraFieldRef extraField = CZExtraFieldCreate();
            
            uint16_t id = CZBinaryReadUInt16LE(bytes + offset + o, &o);
            
            //CZExtraFieldSetHeaderID(extraField, CZBinaryReadUInt16LE(bytes + offset + o, &o));
            
            uint16_t dataSize = CZBinaryReadUInt16LE(bytes + offset + o, &o);
            
//            CZDataRef data = CZDataCreate();
//            CZDataAppendWithBytes(data, bytes + offset + o, dataSize);
//            CZExtraFieldSetDataNoCopy(extraField, data);
            
            
            //CZGlobalHeaderAppendExtraFieldNoCopy(header, extraField);
            
            CZEntryHeaderAppendExtraField(header, id, bytes + offset + o, dataSize);
            
            o += dataSize;
        }
        
        offset += extraFieldLength;
    }
    
    if (fileCommentLength > 0) {
        //CZDataRef fileComment = CZDataCreate();
        //CZDataAppendWithBytes(fileComment, bytes + offset, fileCommentLength);
        //CZGlobalHeaderSetFileCommentNoCopy(header, fileComment);
        
        CZEntryHeaderSetFileComment(header, (const char *)(bytes + offset), fileCommentLength);
        
        offset += fileCommentLength;
    }
    
    free(bytes);
    
    return header;
}

// MARK: - internal

CZStreamRef CZUnzipGetStream(const CZUnzipRef obj) {
    return obj->stream;
}



// MARK: - public

CZUnzipRef CZUnzipCreate(CZStreamRef stream) {
    CZEndOfCentralDirectoryRecordRef record = _CZUnzipSearchEndOfCentralDirectoryRecord(stream);
    if (record == NULL) {
        return NULL;
    }
    
    CZUnzipRef unzip = calloc(1, sizeof(struct CZUnzip));
    unzip->stream = stream;
    unzip->endOfCentralDirectoryRecord = record;
    return unzip;
}

void CZUnzipRelease(CZUnzipRef unzip) {
    if (unzip->endOfCentralDirectoryRecord) {
        CZEndOfCentralDirectoryRecordRelease(unzip->endOfCentralDirectoryRecord);
        unzip->endOfCentralDirectoryRecord = NULL;
    }
    if (unzip->currentHeader) {
        CZEntryHeaderRelease(unzip->currentHeader);
        unzip->currentHeader = NULL;
    }
    free(unzip);
}

const CZEndOfCentralDirectoryRecordRef CZUnzipGetEndOfCentralDirectoryRecord(const CZUnzipRef obj) {
    return obj->endOfCentralDirectoryRecord;
}

void CZUnzipResetIterator(CZUnzipRef unzip) {
    if (unzip->currentHeader) {
        CZEntryHeaderRelease(unzip->currentHeader);
        unzip->currentHeader = NULL;
    }
    unzip->currentHeaderSize = 0;
    unzip->currentHeaderOffset = 0;
}

bool CZUnzipMoveNextEntry(CZUnzipRef unzip) {
    CZEndOfCentralDirectoryRecordInfo info = CZEndOfCentralDirectoryRecordGetInfo(unzip->endOfCentralDirectoryRecord);
    
    if (unzip->currentHeader == NULL) {
        unzip->currentHeaderOffset = info.offsetOfStartOfCentralDirectoryWithRespectToTheStartingDiskNumber;
    } else {
        unzip->currentHeaderOffset += unzip->currentHeaderSize;
    }

    CZStreamSeek(unzip->stream, unzip->currentHeaderOffset, CZStreamSeekOriginBegin);
    
    size_t currentTotalSize = unzip->currentHeaderOffset - info.offsetOfStartOfCentralDirectoryWithRespectToTheStartingDiskNumber;
    size_t maxTotalSize = info.sizeOfTheCentralDirectory;
    if (currentTotalSize >= maxTotalSize) {
        CZUnzipResetIterator(unzip);
        return false;
    }
    
    size_t headerSize = 0;
    if (unzip->currentHeader) {
        CZEntryHeaderRelease(unzip->currentHeader);
    }
    unzip->currentHeader = _CZUnzipCreateCentralDirectoryHeader(unzip->stream, &headerSize);
    unzip->currentHeaderSize = headerSize;
    
    return true;
}

const CZEntryHeaderRef CZUnzipGetCurrentGlobalHeader(const CZUnzipRef obj) {
    return obj->currentHeader;
}

CZUnzipEntryRef CZUnzipGetCurrentEntry(CZUnzipRef unzip) {
    if (unzip->currentHeader == NULL) {
        return NULL;
    }

    CZUnzipEntryRef entry = CZUnzipEntryCreate(unzip);
    return entry;
}

//CZUnzipEntryRef CZUnzipGetCurrentEntry(CZUnzipRef unzip) {
//    return CZUnzipGetCurrentEntryWithDecompressFactory(unzip, NULL);
//}
