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
	return 0이상 :: 상위 디렉토리의 Inode번호
	return -1 :: 실패
*/
int UpdateDirInBlock(char* path, int newInodeNum);
/*	해당 블록의 Dir Entry List에 빈 곳이 있으면
	생성할 이름, Inode번호기록
	int blockNum :: 조회할 Block No.
	char* name :: 항목의 이름
	int inodeNum :: Inode No.
	return 1 :: 성공
	return 0 :: 실패 */
int SetDirFileNameInBlock(int blockNum, char* name, int inodeNum);
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
/* Inode 생성 및 초기화
	int blockNo :: 첫번째 DirEntry가 있는 Block의 No.
	int thisInodeNo :: 생성하는 Inode 번호*/
void CreateDirInode(int blockNo, int thisInodeNo);
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

void PrintInode(int inodeNum);
void PrintDirEntryList(int blockNum);
void PrintIndirectIndexList(int blockNum);

#endif
