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
	
	// 쓴바이트 수
	int CountByte = 0;

	//파일의 fd 가져오기
	FileDesc fileDescItem = pFileDescTable->file[fileDesc];
	//파일의 오프셋 가져오기
	int offset = fileDescItem.fileOffset;

	//Open후 바로 write하는 경우 또는 file사이즈가 offset보다 클 경우
	//Inode에 필요없는 부분 날린다
	LoseFileBlock(offset, fileDescItem.inodeNum);

	//파일의 Inode 가져오기
	Inode* inode = (Inode*)malloc(sizeof(Inode));
	GetInode(fileDescItem.inodeNum, inode);

	

	//추가 시작할 로지컬 블록 순서
	int startLogicalBlockNum = offset / BLOCK_SIZE;
	//추가 마지막 로지컬 블록 순서
	int endLogicalBlockNum = (offset + length) / BLOCK_SIZE;
	//추가 시작할 로지컬 블록의 시작 바이트
	int startByteInStartLogicalBlockNum = offset % BLOCK_SIZE;
	//추가 마지막 로지컬 블록의 마지막 바이트
	int endByteInEndLogicalBlockNum = (offset + length) % BLOCK_SIZE;

	int* indirectBlockNumList = NULL; //인다이렉트블록인덱스 리스트
	//인다이렉트블록이 존재하는 경우 리스트로드
	if (inode->indirBlockPtr > -1) {
		indirectBlockNumList = (int*)malloc(BLOCK_SIZE);
		DevReadBlock(inode->indirBlockPtr, (char*)indirectBlockNumList);
	}
	for (int i = startLogicalBlockNum; i <= endLogicalBlockNum; i++) {
		int blockStartByte = 0;
		int blockNum = -1;
		int blockCpySize = BLOCK_SIZE;
		

		//인다이렉트 진입 순서인 경우
		if (i == NUM_OF_DIRECT_BLOCK_PTR && inode->indirBlockPtr == -1) {
			inode->indirBlockPtr = CreateIndirectBlock();
			indirectBlockNumList = (int*)malloc(BLOCK_SIZE);
			DevReadBlock(inode->indirBlockPtr, (char*)indirectBlockNumList);
		}
		

		//다이렉트블록인 경우 
		if (i < NUM_OF_DIRECT_BLOCK_PTR) {
			blockNum = inode->dirBlockPtr[i];
		}//인다이렉트 블록인경우
		else
			blockNum = indirectBlockNumList[i - NUM_OF_DIRECT_BLOCK_PTR];
		

		// 블록번호가 inode에 할당된게 없으면 할당해 준다. 
		if (blockNum == -1) {
			blockNum = GetFreeBlockNum();
			if (i < NUM_OF_DIRECT_BLOCK_PTR) {
				inode->dirBlockPtr[i] = blockNum;
			}
			else {
				indirectBlockNumList[i- NUM_OF_DIRECT_BLOCK_PTR] = blockNum;
			}
		}
		
		//블록할당이 안된 경우
		if (blockNum == -1) break;

		unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
		DevReadBlock(blockNum, block);

		if(i == startLogicalBlockNum) {
			blockStartByte = startByteInStartLogicalBlockNum;
			blockCpySize -= blockStartByte;
		}
		if (i == endLogicalBlockNum)
			blockCpySize = endByteInEndLogicalBlockNum;
		
		memcpy(block + blockStartByte, pBuffer + CountByte, blockCpySize);
		CountByte += blockCpySize;
		

		DevWriteBlock(blockNum, block);
		SetBlockBitmap(blockNum);
		UpdateNumBlockFSI(1);
		free(block);
		// 마지막 작업 블록일 경우
		if (CountByte == length) break;
		

	}
	if (indirectBlockNumList != NULL) {
		DevWriteBlock(inode->indirBlockPtr, (char*)indirectBlockNumList);
		free(indirectBlockNumList);
	}
	inode->size = offset + length;
	fileDescItem.fileOffset = inode->size;
	PutInode(fileDescItem.inodeNum, inode);
	pFileDescTable->file[fileDesc] = fileDescItem;
	free(inode);
	return CountByte;
}

int		ReadFile(int fileDesc, char* pBuffer, int length)
{
	

	int readByte=0;
	//파일의 fd 가져오기
	FileDesc fileDescItem = pFileDescTable->file[fileDesc];
	//파일의 오프셋 가져오기
	int offset = fileDescItem.fileOffset;
	//Inode 가져오기
	Inode* inode = (Inode*)malloc(sizeof(Inode));
	GetInode(fileDescItem.inodeNum,inode);

	//추가 시작할 로지컬 블록 순서
	int startLogicalBlockNum = offset / BLOCK_SIZE;
	//추가 마지막 로지컬 블록 순서
	int endLogicalBlockNum = (offset + length) / BLOCK_SIZE;
	//추가 시작할 로지컬 블록의 시작 바이트
	int startByteInStartLogicalBlockNum = offset % BLOCK_SIZE;
	//추가 마지막 로지컬 블록의 마지막 바이트
	int endByteInEndLogicalBlockNum = (offset + length) % BLOCK_SIZE;
	
	int* indirectIndexList = NULL;
	if (inode->indirBlockPtr > -1) {
		indirectIndexList = (int*)malloc(BLOCK_SIZE);
		DevReadBlock(inode->indirBlockPtr, (char*)indirectIndexList);
	}

	for (int i = startLogicalBlockNum; i <= endLogicalBlockNum; i++) {
		int blockNum = -1;
		int blockStartByte = 0;
		int blockCpySize = BLOCK_SIZE;

		//다이렉트블록인 경우 
		if (i < NUM_OF_DIRECT_BLOCK_PTR)
			blockNum = inode->dirBlockPtr[i];
		else if (i == NUM_OF_DIRECT_BLOCK_PTR) {
			if (inode->indirBlockPtr == -1) break;
			blockNum = indirectIndexList[i - NUM_OF_DIRECT_BLOCK_PTR];
		}
		else
			blockNum = indirectIndexList[i - NUM_OF_DIRECT_BLOCK_PTR];

		if (blockNum == -1) break;

		//디스크에서 블록 읽어오기
		unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
		DevReadBlock(blockNum, block);
		if (i == startLogicalBlockNum) {
			blockStartByte = startByteInStartLogicalBlockNum;
			blockCpySize -= blockStartByte;
		}
		if (i == endLogicalBlockNum)
			blockCpySize = endByteInEndLogicalBlockNum;

		memcpy(pBuffer+ readByte, block + blockStartByte, blockCpySize);
		readByte += blockCpySize;
		free(block);
	}

	fileDescItem.fileOffset = readByte;
	pFileDescTable->file[fileDesc] = fileDescItem;
	free(inode);
	if(indirectIndexList != NULL) free(indirectIndexList);
	return readByte;
}


int		CloseFile(int fileDesc){
	if (pFileDescTable->file[fileDesc].bUsed == 0) return -1;
	pFileDescTable->file[fileDesc] = SetFileDesc(0, 0, -1);
	return 1;
}

int		RemoveFile(const char* szFileName)
{
	return RemoveItem((char*)szFileName, FILE_TYPE_FILE);
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
	return RemoveItem((char*)szDirName, FILE_TYPE_DIR);
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
