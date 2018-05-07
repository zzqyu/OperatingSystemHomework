#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "fs.h"
#include "own_func.h"

FileDescTable* pFileDescTable = NULL;

/*FileSysinfo, inode bitmap, block bitmap, inode list를 0으로 채워서 초기화*/
void FileSysInit(void)
{
	DevCreateDisk();

    //hw1 int dataRegionFirstIndex = INODELIST_BLOCK_FIRST + INODELIST_BLOCKS;
	int numBlock = BLOCK_SIZE * NUM_BIT_OF_BYTE;

    //디스크 메모리 할당
    //hw1 for(int i = 0; i<dataRegionFirstIndex; i++){
	for (int i = 0; i<numBlock; i++) {
		unsigned char* block = (unsigned char*) malloc (BLOCK_SIZE);
        //non data region 0으로 초기화
        memset(block, 0, BLOCK_SIZE);
        //블록을 저장한다.
        DevWriteBlock(i, block);
		free(block);
    }

}
/*Inode 비트맵의 특정 순서의 비트를 1로 설정
int inodeno: Inode 순서*/
void SetInodeBitmap(int inodeno){
    SetBitmap(INODE_BITMAP_BLOCK_NUM,inodeno,(unsigned char)1);
}
/*Inode 비트맵의 특정 순서의 비트를 0로 설정
int inodeno: Inode 순서*/
void ResetInodeBitmap(int inodeno){
    SetBitmap(INODE_BITMAP_BLOCK_NUM,inodeno,(unsigned char)0);
}
/*Block 비트맵의 특정 순서의 비트를 1로 설정
int blkno: Block 순서*/
void SetBlockBitmap(int blkno){
    SetBitmap(BLOCK_BITMAP_BLOCK_NUM,blkno,(unsigned char)1);
}
/*Block 비트맵의 특정 순서의 비트를 0로 설정
int blkno: Block 순서*/
void ResetBlockBitmap(int blkno){
    SetBitmap(BLOCK_BITMAP_BLOCK_NUM,blkno,(unsigned char)0);
}

/*blkno번째 Inode에 데이터를 넣는다
int blkno: 접근할 Inode의 번호, Inode* pInode: 넣을 Inode 데이터*/
void PutInode(int inodeno, Inode* pInode)
{
	GPInode(inodeno, pInode, (char)0);
}

/*blkno번째 Inode의 데이터를 pInode를 통해 얻는다.
int blkno: 접근할 Inode의 번호, Inode* pInode: 꺼낸 Inode 데이터를 저장할 포인터*/
void GetInode(int inodeno, Inode* pInode)
{
	GPInode(inodeno, pInode, (char)1);
}

/*할당되지 않은 inode 번호를 획득한다. */
int GetFreeInodeNum(void)
{
	GetFreeNum(INODE_BITMAP_BLOCK_NUM);
}

/*할당되지 않은 block 번호를 획득한다. */
int GetFreeBlockNum(void)
{
	return GetFreeNum(BLOCK_BITMAP_BLOCK_NUM);
}

//구현할 소스

int		OpenFile(const char* szFileName, OpenFlag flag)
{

	//상위 디렉토리의 DirEntry에 생성/오픈할 파일이름과 Inode번호 입력
	int newInodeNum = GetFreeInodeNum();
	int parentInodeNum = UpdateDirInBlock((char*)szFileName, newInodeNum, flag);
	//printf("[OpenFile] szFileName=%s   parentInodeNum = %d\n", szFileName, parentInodeNum);
	
	int fileInode = -10000;
	if (parentInodeNum == -1) return -1;
	else if (parentInodeNum >= 10000) {
		fileInode += parentInodeNum;
	}
	else {
		if (flag == OPEN_FLAG_CREATE) {
			fileInode = newInodeNum;
			CreateFileInode(fileInode);
		}
		else if (flag == OPEN_FLAG_READWRITE) {
			return -1;
		}
	}
	//File Descriptor Table에 InodeNum, fileOffset기록하고
	//FDT에 기록된 인덱스 리턴
	if (pFileDescTable == NULL) {
		pFileDescTable = (FileDescTable*)malloc(sizeof(FileDescTable));
		for (int i = 0; i < MAX_FD_ENTRY_LEN; i++) 
			pFileDescTable->file[i] = SetFileDesc(0, 0, -1);
	}

	int fdIndex = 0;
	FileDesc f;
	f.fileOffset = 0;
	f.inodeNum = fileInode;
	//FileDesc fdArr[MAX_FD_ENTRY_LEN];
	for (fdIndex = 0; fdIndex < MAX_FD_ENTRY_LEN; fdIndex++) {
		if ((pFileDescTable->file[fdIndex].bUsed) == 0) {
			pFileDescTable->file[fdIndex] = SetFileDesc(1, 0, fileInode);
			break;
		}
	}

	return fdIndex;
}


int		WriteFile(int fileDesc, char* pBuffer, int length)
{
	return RWFile(fileDesc, pBuffer, length, 1);
}

int		ReadFile(int fileDesc, char* pBuffer, int length)
{
	return RWFile(fileDesc, pBuffer, length, 0);
}


int		CloseFile(int fileDesc){
	if (pFileDescTable->file[fileDesc].bUsed == 0) return -1;
	pFileDescTable->file[fileDesc] = SetFileDesc(0, 0, -1);
	return 1;
}

int		RemoveFile(const char* szFileName)
{
	return RemoveItem((char*)szFileName, FILE_TYPE_FILE, pFileDescTable);
}


int		MakeDir(const char* szDirName)
{
	
	int newInodeNum = GetFreeInodeNum();
	//상위 디렉토리의 DirEntry에 생성할 디렉이름과 Inode번호 입력
	int parentInodeNum = UpdateDirInBlock((char*)szDirName, newInodeNum, 1);
	
	//새로 생성할 항목이 중복이거나 생성할 수 없으면 -1
	if (parentInodeNum >= 10000 || parentInodeNum == -1) return -1;
	int newBlockNum = GetFreeBlockNum();
	//newBlock은 무조건 parentInodeNum뒤에 이유는 새로운 엔트리블록 추가될 수 있음

	//생성할 디렉토리의 DirEntry 리스트 생성/초기화
	CreateDirInBlock(newBlockNum, newInodeNum, parentInodeNum);
	//루트 디렉토리의 Inode 생성/초기화
	CreateDirInode(newBlockNum, newInodeNum, 1);
}


int		RemoveDir(const char* szDirName)
{
	return RemoveItem((char*)szDirName, FILE_TYPE_DIR, NULL);
}

int			EnumerateDirStatus(const char * szDirName, DirEntryInfo * pDirEntry, int dirEntrys)
{

	char* dirName = (char*)malloc(strlen(szDirName) + 2);
	strcpy(dirName, szDirName);
	if (strcmp(dirName, "/") == 0) strcpy(dirName, "/.");
	//상위 디렉토리의 DirEntry에서 접근할 디렉의 Inode번호 가져오기
	int dirInodeNum = -10000;
	int count = -1;
	int resultFunc;

	//if ((resultFunc = UpdateDirInBlock(dirName, -1, 0)) < 10000) {
	if ((dirInodeNum = GetDirInode(dirName, -1)) < 0) {
		
		pDirEntry = NULL;
	}
	else {
		//dirInodeNum += resultFunc;
		if (GetItemType(dirInodeNum) == FILE_TYPE_DIR) {
			int noMean[] = { 0 };
			DirEntryInfo* temp = GetDirEntryInfoList(dirInodeNum, &count, NULL, noMean);
			if (count > dirEntrys)count = dirEntrys;
			memcpy(pDirEntry, temp, sizeof(DirEntryInfo)*count);
			free(temp);
		}
	}
	return count;
}
