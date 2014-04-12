/****************************************************************************\

    WIMGAPI.H

    Copyright (c) Microsoft Corporation.
    All rights reserved.

\****************************************************************************/

#ifndef _WIMGAPI_H_
#define _WIMGAPI_H_

#ifdef __cplusplus
extern "C" {
#endif

//
// Defined Value(s):
//

// WIMCreateFile:
//
#define WIM_GENERIC_READ            GENERIC_READ
#define WIM_GENERIC_WRITE           GENERIC_WRITE
#define WIM_GENERIC_MOUNT           GENERIC_EXECUTE

#define WIM_CREATE_NEW              CREATE_NEW
#define WIM_CREATE_ALWAYS           CREATE_ALWAYS
#define WIM_OPEN_EXISTING           OPEN_EXISTING
#define WIM_OPEN_ALWAYS             OPEN_ALWAYS

typedef enum
{
    WIM_COMPRESS_NONE = 0,
    WIM_COMPRESS_XPRESS,
    WIM_COMPRESS_LZX
};

typedef enum
{
    WIM_CREATED_NEW = 0,
    WIM_OPENED_EXISTING
};

// WIMCreateFile, WIMCaptureImage, WIMApplyImage, WIMMountImageHandle flags:
//
#define WIM_FLAG_RESERVED           0x00000001
#define WIM_FLAG_VERIFY             0x00000002
#define WIM_FLAG_INDEX              0x00000004
#define WIM_FLAG_NO_APPLY           0x00000008
#define WIM_FLAG_NO_DIRACL          0x00000010
#define WIM_FLAG_NO_FILEACL         0x00000020
#define WIM_FLAG_SHARE_WRITE        0x00000040
#define WIM_FLAG_FILEINFO           0x00000080
#define WIM_FLAG_NO_RP_FIX          0x00000100
#define WIM_FLAG_MOUNT_READONLY     0x00000200

// WIMGetMountedImageList flags
//
#define WIM_MOUNT_FLAG_MOUNTED              0x00000001
#define WIM_MOUNT_FLAG_MOUNTING             0x00000002
#define WIM_MOUNT_FLAG_REMOUNTABLE          0x00000004
#define WIM_MOUNT_FLAG_INVALID              0x00000008
#define WIM_MOUNT_FLAG_NO_WIM               0x00000010
#define WIM_MOUNT_FLAG_NO_MOUNTDIR          0x00000020
#define WIM_MOUNT_FLAG_MOUNTDIR_REPLACED    0x00000040
#define WIM_MOUNT_FLAG_READWRITE            0x00000100

// WIMCommitImageHandle flags
//
#define WIM_COMMIT_FLAG_APPEND      0x00000001

// WIMSetReferenceFile
//
#define WIM_REFERENCE_APPEND        0x00010000
#define WIM_REFERENCE_REPLACE       0x00020000

// WIMExportImage
//
#define WIM_EXPORT_ALLOW_DUPLICATES 0x00000001
#define WIM_EXPORT_ONLY_RESOURCES   0x00000002
#define WIM_EXPORT_ONLY_METADATA    0x00000004

// WIMRegisterMessageCallback:
//
#define INVALID_CALLBACK_VALUE      0xFFFFFFFF

// WIMCopyFile
//
#define WIM_COPY_FILE_RETRY         0x01000000

// WIMDeleteImageMounts
//
#define WIM_DELETE_MOUNTS_ALL       0x00000001

// WIMMessageCallback Notifications:
//
typedef enum
{
    WIM_MSG = WM_APP + 0x1476,
    WIM_MSG_TEXT,
    WIM_MSG_PROGRESS,
    WIM_MSG_PROCESS,
    WIM_MSG_SCANNING,
    WIM_MSG_SETRANGE,
    WIM_MSG_SETPOS,
    WIM_MSG_STEPIT,
    WIM_MSG_COMPRESS,
    WIM_MSG_ERROR,
    WIM_MSG_ALIGNMENT,
    WIM_MSG_RETRY,
    WIM_MSG_SPLIT,
    WIM_MSG_FILEINFO,
    WIM_MSG_INFO,
    WIM_MSG_WARNING,
    WIM_MSG_CHK_PROCESS,
    WIM_MSG_WARNING_OBJECTID,
    WIM_MSG_STALE_MOUNT_DIR,
    WIM_MSG_STALE_MOUNT_FILE,
    WIM_MSG_MOUNT_CLEANUP_PROGRESS,
    WIM_MSG_CLEANUP_SCANNING_DRIVE,
    WIM_MSG_IMAGE_ALREADY_MOUNTED,
    WIM_MSG_CLEANUP_UNMOUNTING_IMAGE,
    WIM_MSG_QUERY_ABORT
};

//
// WIMMessageCallback Return codes:
//
#define WIM_MSG_SUCCESS     ERROR_SUCCESS
#define WIM_MSG_DONE        0xFFFFFFF0
#define WIM_MSG_SKIP_ERROR  0xFFFFFFFE
#define WIM_MSG_ABORT_IMAGE 0xFFFFFFFF

//
// WIM_INFO dwFlags values:
//
#define WIM_ATTRIBUTE_NORMAL        0x00000000
#define WIM_ATTRIBUTE_RESOURCE_ONLY 0x00000001
#define WIM_ATTRIBUTE_METADATA_ONLY 0x00000002
#define WIM_ATTRIBUTE_VERIFY_DATA   0x00000004
#define WIM_ATTRIBUTE_RP_FIX        0x00000008
#define WIM_ATTRIBUTE_SPANNED       0x00000010
#define WIM_ATTRIBUTE_READONLY      0x00000020

//
// The WIM_INFO structure used by WIMGetAttributes:
//
typedef struct _WIM_INFO
{
    WCHAR  WimPath[MAX_PATH];
    GUID   Guid;
    DWORD  ImageCount;
    DWORD  CompressionType;
    USHORT PartNumber;
    USHORT TotalParts;
    DWORD  BootIndex;
    DWORD  WimAttributes;
    DWORD  WimFlagsAndAttr;
} WIM_INFO, *PWIM_INFO, *LPWIM_INFO;

//
// The WIM_MOUNT_LIST structure used for getting the list of mounted images.
//
typedef struct _WIM_MOUNT_LIST
{
    WCHAR  WimPath[MAX_PATH];
    WCHAR  MountPath[MAX_PATH];
    DWORD  ImageIndex;
    BOOL   MountedForRW;
} WIM_MOUNT_LIST, *PWIM_MOUNT_LIST, *LPWIM_MOUNT_LIST,
  WIM_MOUNT_INFO_LEVEL0, *PWIM_MOUNT_INFO_LEVEL0, LPWIM_MOUNT_INFO_LEVEL0;

//
// Define new WIM_MOUNT_INFO_LEVEL1 structure with additional data...
//
typedef struct _WIM_MOUNT_INFO_LEVEL1
{
    WCHAR  WimPath[MAX_PATH];
    WCHAR  MountPath[MAX_PATH];
    DWORD  ImageIndex;
    DWORD  MountFlags;
} WIM_MOUNT_INFO_LEVEL1, *PWIM_MOUNT_INFO_LEVEL1, *LPWIM_MOUNT_INFO_LEVEL1;

//
// Define enumeration for WIMGetMountedImageInfo to determine structure to use...
//
typedef enum _MOUNTED_IMAGE_INFO_LEVELS 
{
    MountedImageInfoLevel0,
    MountedImageInfoLevel1,
    MountedImageInfoLevelInvalid
} MOUNTED_IMAGE_INFO_LEVELS;

//
// Exported Function Prototypes:
//
HANDLE
WINAPI
WIMCreateFile(
    __in      PCWSTR pszWimPath,
    __in      DWORD  dwDesiredAccess,
    __in      DWORD  dwCreationDisposition,
    __in      DWORD  dwFlagsAndAttributes,
    __in      DWORD  dwCompressionType,
    __out_opt PDWORD pdwCreationResult
    );

BOOL
WINAPI
WIMCloseHandle(
    __in HANDLE hObject
    );

BOOL
WINAPI
WIMSetTemporaryPath(
    __in HANDLE hWim,
    __in PCWSTR pszPath
    );

BOOL
WINAPI
WIMSetReferenceFile(
    __in HANDLE hWim,
    __in PCWSTR pszPath,
    __in DWORD  dwFlags
    );

BOOL
WINAPI
WIMSplitFile(
    __in    HANDLE         hWim,
    __in    PCWSTR         pszPartPath,
    __inout PLARGE_INTEGER pliPartSize,
    __in    DWORD          dwFlags
    );

BOOL
WINAPI
WIMExportImage(
    __in HANDLE hImage,
    __in HANDLE hWim,
    __in DWORD  dwFlags
    );

BOOL
WINAPI
WIMDeleteImage(
    __in HANDLE hWim,
    __in DWORD  dwImageIndex
    );

DWORD
WINAPI
WIMGetImageCount(
    __in HANDLE hWim
    );

BOOL
WINAPI
WIMGetAttributes(
    __in                    HANDLE     hWim,
    __out_bcount(cbWimInfo) PWIM_INFO  pWimInfo,
    __in                    DWORD      cbWimInfo
    );

BOOL
WINAPI
WIMSetBootImage(
    __in HANDLE hWim,
    __in DWORD  dwImageIndex
    );

HANDLE
WINAPI
WIMCaptureImage(
    __in HANDLE hWim,
    __in PCWSTR pszPath,
    __in DWORD  dwCaptureFlags
    );

HANDLE
WINAPI
WIMLoadImage(
    __in HANDLE hWim,
    __in DWORD  dwImageIndex
    );

BOOL
WINAPI
WIMApplyImage(
    __in     HANDLE hImage,
    __in_opt PCWSTR pszPath,
    __in     DWORD  dwApplyFlags
    );

BOOL
WINAPI
WIMGetImageInformation(
    __in  HANDLE hImage,
    __out PVOID  *ppvImageInfo,
    __out PDWORD pcbImageInfo
    );

BOOL
WINAPI
WIMSetImageInformation(
    __in                     HANDLE hImage,
    __in_bcount(cbImageInfo) PVOID  pvImageInfo,
    __in                     DWORD  cbImageInfo
    );

DWORD
WINAPI
WIMGetMessageCallbackCount(
    __in_opt HANDLE hWim
    );

DWORD
WINAPI
WIMRegisterMessageCallback(
    __in_opt HANDLE  hWim,
    __in     FARPROC fpMessageProc,
    __in_opt PVOID   pvUserData
    );

BOOL
WINAPI
WIMUnregisterMessageCallback(
    __in_opt HANDLE  hWim,
    __in_opt FARPROC fpMessageProc
    );

DWORD
WINAPI
WIMMessageCallback(
    __in DWORD  dwMessageId,
    __in WPARAM wParam,
    __in LPARAM lParam,
    __in PVOID  pvUserData
    );

BOOL
WINAPI
WIMCopyFile(
    __in     PCWSTR             pszExistingFileName,
    __in     PCWSTR             pszNewFileName,
    __in_opt LPPROGRESS_ROUTINE pProgressRoutine,
    __in_opt PVOID              pvData,
    __in_opt PBOOL              pbCancel,
    __in     DWORD              dwCopyFlags
    );

BOOL
WINAPI
WIMMountImage(
    __in     PCWSTR pszMountPath,
    __in     PCWSTR pszWimFileName,
    __in     DWORD  dwImageIndex,
    __in_opt PCWSTR pszTempPath
    );

BOOL
WINAPI
WIMMountImageHandle(
    __in HANDLE hImage,
    __in PCWSTR pszMountPath,
    __in DWORD  dwMountFlags
    );

BOOL
WINAPI
WIMRemountImage(
    __in PCWSTR pszMountPath,
    __in DWORD  dwFlags
    );

BOOL
WINAPI
WIMCommitImageHandle(
    __in      HANDLE  hImage,
    __in      DWORD   dwCommitFlags,
    __out_opt PHANDLE phNewImageHandle
    );

BOOL
WINAPI
WIMUnmountImage(
    __in     PCWSTR pszMountPath,
    __in_opt PCWSTR pszWimFileName,
    __in     DWORD  dwImageIndex,
    __in     BOOL   bCommitChanges
    );

BOOL
WINAPI
WIMUnmountImageHandle(
    __in HANDLE hImage,
    __in DWORD  dwUnmountFlags
    );

BOOL
WINAPI
WIMGetMountedImages(
    __out_bcount_opt(*pcbMountList) PWIM_MOUNT_LIST pMountList,
    __inout                         PDWORD          pcbMountListLength
    );

BOOL
WINAPI
WIMGetMountedImageInfo(
    __in         MOUNTED_IMAGE_INFO_LEVELS fInfoLevelId,
    __out                           PDWORD pdwImageCount,
    __out_bcount_opt(*pcbMountInfo) PVOID  pMountInfo,
    __in                            DWORD  cbMountInfoLength,
    __out                           PDWORD pcbReturnLength
    );

BOOL
WINAPI
WIMGetMountedImageInfoFromHandle(
    __in                            HANDLE hImage,
    __in         MOUNTED_IMAGE_INFO_LEVELS fInfoLevelId,
    __out_bcount_opt(*pcbMountInfo) PVOID  pMountInfo,
    __in                            DWORD  cbMountInfoLength,
    __out                           PDWORD pcbReturnLength
    );

BOOL
WINAPI
WIMGetMountedImageHandle(
    __in  PCWSTR  pszMountPath,
    __in  DWORD   dwFlags,
    __out PHANDLE phWimHandle,
    __out PHANDLE phImageHandle 
    );

BOOL
WINAPI
WIMDeleteImageMounts(
    __in DWORD dwDeleteFlags
    );

BOOL
WINAPI
WIMRegisterLogFile(
    __in PCWSTR pszLogFile,
    __in DWORD  dwFlags
    );

BOOL
WINAPI
WIMUnregisterLogFile(
    __in PCWSTR pszLogFile
    );

BOOL
WINAPI
WIMExtractImagePath(
    __in HANDLE hImage,
    __in PCWSTR pszImagePath,
    __in PCWSTR pszDestinationPath,
    __in DWORD  dwExtractFlags
    );

BOOL
WINAPI
WIMInitFileIOCallbacks(
    __in_opt PVOID pCallbacks
    );

BOOL
WINAPI
WIMSetFileIOCallbackTemporaryPath(
    __in_opt PCWSTR pszPath
    );

//
// File I/O callback prototypes
//
typedef VOID * PFILEIOCALLBACK_SESSION;

typedef
PFILEIOCALLBACK_SESSION
(CALLBACK * FileIOCallbackOpenFile)(
    __in PCWSTR pszFileName
    );

typedef
BOOL
(CALLBACK * FileIOCallbackCloseFile)(
    __in PFILEIOCALLBACK_SESSION hFile
    );

typedef
BOOL
(CALLBACK * FileIOCallbackReadFile)(
    __in PFILEIOCALLBACK_SESSION hFile,
    __in PVOID pBuffer,
    __in DWORD nNumberOfBytesToRead,
    __in PDWORD pNumberOfBytesRead,
    __in LPOVERLAPPED pOverlapped
    );

typedef
BOOL
(CALLBACK * FileIOCallbackSetFilePointer)(
    __in PFILEIOCALLBACK_SESSION hFile,
    __in LARGE_INTEGER liDistanceToMove,
    __in PLARGE_INTEGER pNewFilePointer,
    __in DWORD dwMoveMethod
    );

typedef
BOOL
(CALLBACK * FileIOCallbackGetFileSize)(
    __in HANDLE hFile,
    __in PLARGE_INTEGER pFileSize
    );

typedef struct _SFileIOCallbackInfo
{
    FileIOCallbackOpenFile       pfnOpenFile;
    FileIOCallbackCloseFile      pfnCloseFile;
    FileIOCallbackReadFile       pfnReadFile;
    FileIOCallbackSetFilePointer pfnSetFilePointer;
    FileIOCallbackGetFileSize    pfnGetFileSize;
} SFileIOCallbackInfo;

#ifdef __cplusplus
}
#endif

#endif // _WIMGAPI_H_
