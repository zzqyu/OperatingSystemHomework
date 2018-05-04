#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "Disk.h"


// ------- Caution -----------------------------------------
#define FS_DISK_CAPACITY	(8388608) /* 8M */
#define MAX_FD_ENTRY_LEN		(64)


#define NUM_OF_INODE_PER_BLK	(BLOCK_SIZE / sizeof(Inode))
#define NUM_OF_DIRENT_PER_BLK	(BLOCK_SIZE / sizeof(DirEntry))


#define NUM_OF_DIRECT_BLK_PTR	(2)
#define MAX_INDEX_OF_DIRBLK	(NUM_OF_DIRECT_PER_BLK)
#define MAX_NAME_LEN      (12)


#define FILESYS_INFO_BLOCK    (0) /* file system info block no. */
#define BLOCK_BITMAP_BLK_NUM  (1) /* block bitmap block no. */
#define INODE_BITMAP_BLK_NUM  (2) /* inode bitmap block no. */
#define INODELIST_BLK_FIRST   (3) /* the first block no. of inode list */
#define INODELIST_BLKS        (16) /* the number of blocks in inode list */


// ----------------------------------------------------------


typedef enum __openFlag {
	OPEN_FLAG_READWRITE,
	OPEN_FLAG_CREATE
} OpenFlag;


typedef enum __fileType {
	FILE_TYPE_FILE,  // regular file
	FILE_TYPE_DIR,   // directory file
	FILE_TYPE_DEV    // device file
} FileType;



typedef enum __fileMode {
	FILE_MODE_READONLY,
	FILE_MODE_READWRITE,
	FILE_MODE_EXEC
}FileMode;



typedef struct  __dirEntry {
     char name[MAX_NAME_LEN];        // file name
     int inodeNum; 
} DirEntry;



typedef enum __mountType {
    MT_TYPE_FORMAT,
    MT_TYPE_READWRITE
} MountType;



typedef struct _FileSysInfo {
	int blocks;              // 디스크에 저장된 전체 블록 개수
	int rootInodeNum;        // 루트 inode의 번호
	int diskCapacity;        // 디스크 용량 (Byte 단위)
	int numAllocBlocks;      // 파일 또는 디렉토리에 할당된 블록 개수
	int numFreeBlocks;       // 할당되지 않은 블록 개수
	int numAllocInodes;       // 할당된 inode 개수 
	int blockBitmapBlock;     // block bitmap의 시작 블록 번호
	int inodeBitmapBlock;     // inode bitmap의 시작 블록 번호
	int inodeListBlock;		// inode list의 시작 블록 주소
	int dataRegionBlock;		// inode list의 시작 블록 주소
} FileSysInfo;




typedef struct _Inode {
	short 	size;              // 파일 크기
	short 	type;              // 파일 타입
    int 	dirBlkPtr[NUM_OF_DIRECT_BLK_PTR];	// Direct block pointers
    int   	indirBlkPointer;	// Single indirect block pointer
} Inode;



typedef struct __fileDesc {
	int	bUsed;
	int	fileOffset;
	int	inodeNum;
}FileDesc;

typedef struct __fileDescTable {
	FileDesc	file[MAX_FD_ENTRY_LEN];
}FileDescTable;

extern int		OpenFile(const char* szFileName, OpenFlag flag);
extern int		WriteFile(int fileDesc, char* pBuffer, int length);
extern int		ReadFile(int fileDesc, char* pBuffer, int length);
extern int		CloseFile(int fileDesc);
extern int		RemoveFile(const char* szFileName);
extern int		MakeDir(const char* szDirName);
extern int		RemoveDir(const char* szDirName);
extern void		EnumerateDirStatus(const char* szDirName, DirEntry* pDirEntry, int* pNum);
extern void		Mount(MountType type);
extern void		Unmount(void);

extern FileDescTable* pFileDescTable;
extern FileSysInfo* pFileSysInfo;

/*  File system internal functions */


void FileSysInit(void);
void SetInodeBitmap(int inodeno);
void ResetInodeBitmap(int inodeno);
void SetBlockBitmap(int blkno);
void ResetBlockBitmap(int blkno);
void PutInode(int inodeno, Inode* pInode);
void GetInode(int inodeno, Inode* pInode);
int GetFreeInodeNum(void);
int GetFreeBlockNum(void);



#endif /* FILESYSTEM_H_ */

//hw1 source
/*
#ifndef __FS_H__
#define __FS_H__
#define NUM_OF_BLK_PTR  (14)

#define INODE_BITMAP_BLK_NUM  (1) // inode bitmap block no. 
#define BLOCK_BITMAP_BLK_NUM  (2) // block bitmap block no. 
#define INODELIST_BLK_FIRST   (3) // the first block no. of inode list 
#define INODELIST_BLKS        (4) // the number of blocks in inode list 


typedef struct __Inode {
	int			allocBlocks;
	int			type;
	int			blockPointer[NUM_OF_BLK_PTR];	// Direct block pointers
} Inode;

void FileSysInit(void);
void SetInodeBitmap(int inodeno);
void ResetInodeBitmap(int inodeno);
void SetBlockBitmap(int blkno);
void ResetBlockBitmap(int blkno);
void PutInode(int inodeno, Inode* pInode);
void GetInode(int inodeno, Inode* pInode);
int GetFreeInodeNum(void);
int GetFreeBlockNum(void);
#endif





*/