//
//  CZTypes.h
//  CoreZipArchive
//
//  Created by Yasuhiro Hatta on 2017/05/15.
//  Copyright © 2017 yaslab. All rights reserved.
//

#ifndef CZTypes_h
#define CZTypes_h

#include "CZUtility.h"

#define CZLocalHeaderSignature              0x04034b50L
#define CZDataDescriptorSignature           0x08074b50L
#define CZGlobalHeaderSignature             0x02014b50L
#define CZEndOfCentralDirectorySignature    0x06054b50L

// -----------------------------------------------------------------------------
// MARK: - CZExtraField

typedef struct CZExtraField * CZExtraFieldRef;

uint16_t CZExtraFieldGetHeaderID(const CZExtraFieldRef _Nonnull obj);

swift_int_t CZExtraFieldGetDataLength(const CZExtraFieldRef _Nonnull obj);
const uint8_t * _Nonnull CZExtraFieldGetData(const CZExtraFieldRef _Nonnull obj);

// -----------------------------------------------------------------------------
// MARK: - CZDataDescriptor

typedef struct {
    
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    
} CZDataDescriptor;

// -----------------------------------------------------------------------------
// MARK: - CZEntryHeader

typedef struct CZEntryHeader * CZEntryHeaderRef;

typedef struct {
    
    // version made by (lower byte)
    uint8_t versionMadeBy;
    // version made by (upper byte)
    uint8_t systemInfo;
    
    // version needed to extract
    uint16_t versionNeededToExtract;
    // general purpose bit flag
    uint16_t flags;
    // compression method
    uint16_t method;
    // last mod file time
    uint16_t time;
    // last mod file date
    uint16_t date;
    // crc-32
    uint32_t crc;
    // compressed size
    uint32_t compressedSize;
    // uncompressed size
    uint32_t size;
    
    // disk number start
    uint16_t diskNumberStart;
    // internal file attributes
    uint16_t internalFileAttributes;
    // external file attributes
    uint32_t externalFileAttributes;
    // relative offset of local header
    uint32_t localHeaderOffset;
    
} CZEntryHeaderInfo;

CZEntryHeaderRef _Nonnull CZEntryHeaderCreate(void);
void CZEntryHeaderRelease(CZEntryHeaderRef _Nonnull obj);

CZEntryHeaderInfo CZEntryHeaderGetInfo(const CZEntryHeaderRef _Nonnull obj);
void CZEntryHeaderSetInfo(CZEntryHeaderRef _Nonnull obj, CZEntryHeaderInfo info);

swift_int_t CZEntryHeaderGetFileNameLength(const CZEntryHeaderRef _Nonnull obj);
void CZEntryHeaderGetFileName(const CZEntryHeaderRef _Nonnull obj, char * _Nonnull buffer, swift_int_t bufferSize);
void CZEntryHeaderSetFileName(CZEntryHeaderRef _Nonnull obj, const char * _Nonnull str, swift_int_t length);

swift_int_t CZEntryHeaderGetExtraFieldCount(const CZEntryHeaderRef _Nonnull obj);

const CZExtraFieldRef _Nullable CZEntryHeaderGetExtraFieldAtIndex(const CZEntryHeaderRef _Nonnull obj, swift_int_t index);
swift_int_t CZEntryHeaderAppendExtraField(CZEntryHeaderRef _Nonnull obj, uint16_t id, const uint8_t * _Nonnull data, swift_int_t length);

swift_int_t CZEntryHeaderGetFileCommentLength(const CZEntryHeaderRef _Nonnull obj);
void CZEntryHeaderGetFileComment(const CZEntryHeaderRef _Nonnull obj, char * _Nonnull buffer, swift_int_t bufferSize);
void CZEntryHeaderSetFileComment(CZEntryHeaderRef _Nonnull obj, const char * _Nonnull str, swift_int_t length);

// -----------------------------------------------------------------------------
// MARK: - CZEndOfCentralDirectoryRecord

typedef struct CZEndOfCentralDirectoryRecord * CZEndOfCentralDirectoryRecordRef;

typedef struct {
    
    uint16_t numberOfThisDisk;
    uint16_t numberOfTheDiskWithTheStartOfTheCentralDirectory;
    uint16_t totalNumberOfEntriesInTheCentralDirectoryOnThisDisk;
    uint16_t totalNumberOfEntriesInTheCentralDirectory;
    uint32_t sizeOfTheCentralDirectory;
    uint32_t offsetOfStartOfCentralDirectoryWithRespectToTheStartingDiskNumber;
    
} CZEndOfCentralDirectoryRecordInfo;

CZEndOfCentralDirectoryRecordRef _Nonnull CZEndOfCentralDirectoryRecordCreate(void);
void CZEndOfCentralDirectoryRecordRelease(CZEndOfCentralDirectoryRecordRef _Nonnull obj);

CZEndOfCentralDirectoryRecordInfo CZEndOfCentralDirectoryRecordGetInfo(const CZEndOfCentralDirectoryRecordRef _Nonnull obj);
void CZEndOfCentralDirectoryRecordSetInfo(CZEndOfCentralDirectoryRecordRef _Nonnull obj, CZEndOfCentralDirectoryRecordInfo info);

swift_int_t CZEndOfCentralDirectoryRecordGetFileCommnetLength(const CZEndOfCentralDirectoryRecordRef _Nonnull obj);
void CZEndOfCentralDirectoryRecordGetFileCommnet(const CZEndOfCentralDirectoryRecordRef _Nonnull obj, char * _Nonnull buffer, swift_int_t bufferSize);
void CZEndOfCentralDirectoryRecordSetFileCommnet(CZEndOfCentralDirectoryRecordRef _Nonnull obj, const char * _Nonnull str, swift_int_t length);

#endif /* CZTypes_h */
