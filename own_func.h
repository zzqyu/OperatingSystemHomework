#ifndef __OWN_FUNC_H__
#define __OWN_FUNC_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "fs.h"

#define NUM_BIT_OF_BYTE (8)

//#define FSI_NUM  0 /*File System Infomation block index num*/
//#define DATA_REGION_FIRST  7 /* the first block no. of data region*/
//#define INODE_SIZE  64 /* size(byte) of Inode*/

//#define INODE_NUM_OF_BLOCK 4 /*한 블록에 들어가는 inode개수*/
//#define INODE_ITEM_NUM  INODE_NUM_OF_BLOCK*INODELIST_BLKS /*Inode 갯수*/

/*bit내용을 배열로 바꿔 리턴
int digitSize: 몇자리수까지 출력하는지.. , unsigned char* b: 출력할 변수*/
unsigned char* BlockToBinary(int digitSize, unsigned char* b);

/*inodeSeq번째 노드를 get, put
int inodeSeq: 노드의 순서, Inode* pInode: 사용할 Inode, char isGet: Get하려면 1 Put하려면 0*/
void GPInode(int inodeSeq, Inode* pInode, char isGet);

/*blkno번째 블록에서 첫번째로 0인 bit인덱스번호 리턴
int blkno: 접근할 블록번호*/
int GetFreeNum(int blkno);

/* 비트맵 블록의 어떤 인덱스에 접근해 0이나 1로 세팅함.
int blkno: 접근할 블록번호, int bitmapIndex: 비트맵의 인덱스번호, unsigned char bit: */
void SetBitmap(int blkno, int bitmapIndex, unsigned char bit);

/* Operation <<  : target << manyShiftBit
unsigned char* target: 연산할 대상, int targetByteSize: 연산할 대상의 byte size, int manyShiftBit: 몇비트씩 시프트할지*/
unsigned char* LeftShift(unsigned char* target, int targetByteSize, int manyShiftBit);

/* Operation >>  : target >> manyShiftBit
unsigned char* target: 연산할 대상, int targetByteSize: 연산할 대상의 byte size, int manyShiftBit: 몇비트씩 시프트할지*/
unsigned char* RightShift(unsigned char* target, int targetByteSize, int manyShiftBit);

/* Operation |=
lValue |= rValue , int valueByteSize : 바이트 크기 */
void OrEqual(unsigned char * lValue, unsigned char * rValue, int valueByteSize);

/* Operation &=
lValue &= rValue , int valueByteSize : 바이트 크기 */
void AndEqual(unsigned char * lValue, unsigned char * rValue, int valueByteSize);

/* Operation |
lValue | rValue , int valueByteSize : 바이트 크기 */
unsigned char* OperOr(unsigned char* value1, unsigned char* value2, int valueByteSize);

/* Operation &
lValue & rValue , int valueByteSize : 바이트 크기 */
unsigned char* OperAnd(unsigned char* value1, unsigned char* value2, int valueByteSize);

/* Operation (1 << fillBitSize) -1
(1 << fillBitSize) -1 , int valueByteSize : 바이트 크기 */
unsigned char* FillBit(int fillBitSize, int valueByteSize);


//=======================HW2 ZONE =============================

/*	DirEntry 항목을 설정하는 함수
	char* name :: 생성할 항목(디렉토리/파일) 이름
	int inodeNum :: 생성할 항목의 Inode 번호
	return 설정된 DriEntryItem  */
DirEntry SetDirEntry(char* name, int inodeNum);
/*	FileDesc 항목을 설정하는 함수
 int bUsed, int offset, int inodeNum*/
FileDesc SetFileDesc(int bUsed, int offset, int inodeNum);
/*	생성할 디렉토리의 엔트리 리스트를 초기화
	int blockNo :: 디렉토리 엔트리를 만들 Block No.
	int thisInodeNo :: 생성하는 디렉토리의 Inode No.
	int parentInodeNo :: 상위 디렉토리의 Inode No.
	return void */
void CreateDirInBlock(int blockNo, int thisInodeNo, int parentInodeNo);
/*	Indirect Block 생성 및 초기화
	return 생성된 Indirect Block의 Block No.*/
int CreateIndirectBlock();
/*	Second Direct Block 생성 및 초기화
	return 생성된 Seocond Direct Block의 Block No.*/
int AddDirEntry();
/*	생성할 항목(디렉토리/파일)의 상위 디렉토리의 엔트리를 
	루트부터 순차적으로 찾아서 이름, Inode번호기록
	char* path :: 생성할 항목의 절대경로
	int newInodeNum :: 생성할 항목에 배정할 Inode No.
	int isUpdate :: 업데이트를 할건지, 중복검사만 할건지
	return 0이상 :: 상위 디렉토리의 Inode번호
	return -1 :: 실패
	return 10000+중복항목InodeNo :: 항목명 중복 */
int UpdateDirInBlock(char* path, int newInodeNum, int isUpdate);
/*	해당 블록의 Dir Entry List에 빈 곳이 있으면
	생성할 이름, Inode번호기록
	int blockNum :: 조회할 Block No.
	char* name :: 항목의 이름
	int inodeNum :: Inode No.
	int isUpdate :: 업데이트를 할건지, 중복검사만 할건지
	return 0이상 :: 성공 추가한Inode No
	return -1 :: 실패 
	return 10000+중복항목InodeNo :: 항목명 중복 */
int SetDirFileNameInBlock(int blockNum, char* name, int inodeNum, int isUpdate);
/*	추가할 항목의 상위디렉토리의 Inode No.를 얻어오는 함수
	char* path :: 추가 요청한 절대경로
	int inodeNum :: (최초 실행 시 -1) 조회할 디렉토리의 Inode No.
	return 추가할 항목의 상위디렉토리의 Inode No.
	return -1 :: 실패*/
int GetDirInode(char* path, int inodeNum);
/*	디렉토리의 모든 엔트리 리스트에서 이름과 일치하는 InodeNo 리턴
	char* dirName :: 검색할 디렉토리 리턴
	int inodeNum :: 검사하는 디렉토리의 Inode No.
	return 디렉이름과 일치하는 InodeNo
	return -1 :: 실패*/
int GetInodeNumInAllDirEntry(char* dirName, int inodeNum);
/*	디렉토리의 모든 Indirect 엔트리리스트에서 이름과 일치하는 InodeNo 리턴
	char* dirName :: 검색할 디렉토리 리턴
	int indirectBlockNum :: 검사하는 인다이렉트 index block의 Inode No.
	return 디렉이름과 일치하는 InodeNo
	return -1 :: 실패*/
int GetInodeNumInAllIndirectDirEntry(char* dirName, int indirectBlockNum);
/*	한 블록의 모든 엔트리리스트에서 이름과 일치하는 InodeNo 리턴
	char* dirName :: 검색할 디렉토리 리턴
	int inodeNum :: 검사하는 디렉토리의 Inode No.
	return 디렉이름과 일치하는 InodeNo
	return -1 :: 실패*/
int GetInodeNumInDirEntry(char* dirName, int blockNum);
/* Inode 생성 및 초기화(directory)
	int blockNo :: 첫번째 DirEntry가 있는 Block의 No.
	int thisInodeNo :: 생성하는 Inode 번호
	int isNotRoot :: 생성하는 Inode가 관리하는 디렉토리가 루트가 아닌지 확인*/
void CreateDirInode(int blockNo, int thisInodeNo, int isNotRoot);
/* Inode 생성 및 초기화(File)
int inodeNum :: 생성하는 Inode 번호*/
void CreateFileInode(int inodeNum);
/*	Inode가 관리하는 디렉토리의 엔트리에 항목이 추가/삭제될때 수행하는 업데이트
int inodeNum :: 업데이트 할 Inode No.
int addSize :: 추가/삭제 여부*/
void UpdateDirInode(int inodeNum, int isAdd);
/*	Inode가 관리하는 디렉토리/파일의 사이즈를 업데이트 함
	int inodeNum :: 업데이트 할 Inode No.
	int addSize :: 가감할 사이즈*/
void UpdateSizeOfInode(int inodeNum, int addSize);
/*	Open후 바로 write하는 경우 또는 file사이즈가 offset보다 클 경우 
	Inode에 필요없는 부분 날린다. 
	int offset :: 날리기 시작하는 부분
	int inodeNum :: 블록을 관리하는 InodeNo. */
void LoseFileBlock(int offset, int inodeNum);
int ResetDir(int inodeNum, FileType type);
int ResetFile(int inodeNum, FileType type);
void RemoveLogicalBlock(int itemInodeNum, int removeLogicalBlockNum);

int RemoveItem(char* itemName, FileType fType, FileDescTable* fdTable);
/*	해당 디렉토리 INode의 DirEntryList 가져오는 함수
	int inodeNum :: 디렉토리의 Inode No.
	DirEntry* pDirEntry :: DirEntryList 담을 변수
	int* logicalBlockNums :: Logical블록넘버리스트
	return Logical블록넘버 개수 */
DirEntryInfo* GetDirEntryInfoList(int inodeNum, int* count, int* logicalBlockNums, int* logicalCount);

int	RWFile(int fileDesc, char* pBuffer, int length, int isWrite);

int GetItemType(int inodeNum);
/* FSI 초기화*/
void InitFileSysInfo();
/* FSI의 numAllocBlocks, numFreeBlocks 업데이트
	int isAdd :: 1이면 할당 블록 1증가, 0이면 1감소*/
void UpdateNumBlockFSI(int isAdd);
/* FSI의 numAllocInodes 업데이트
int isAdd :: 1이면 할당 Inode 1증가, 0이면 1감소*/
void UpdateNumInodeFSI(int isAdd);


/*이 경로의 상위 디렉토리가 루트인지 확인*/
int isParentRoot(char* path);

char* GetItemName(char* path);

void PrintInode(int inodeNum);
void PrintDirEntryList(int blockNum);
void PrintIndirectIndexList(int blockNum);

void printFileSys();
void PrintEntryInfoList(int size, DirEntryInfo* dInfoEntry);
#endif
