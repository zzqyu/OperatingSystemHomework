#ifndef __OWN_FUNC_H__
#define __OWN_FUNC_H__

#include <stdlib.h>
#include <string.h>

#define INODE_ITEM_NUM  32 /*Inode 갯수*/
#define FSI_NUM  0 /*File System Infomation block index num*/
#define DATA_REGION_FIRST  7 /* the first block no. of data region*/
#define INODE_SIZE  64 /* size(byte) of Inode*/

/*bit내용을 배열로 바꿔 리턴
int digitSize: 몇자리수까지 출력하는지.. , unsigned char* b: 출력할 변수*/
//unsigned char* BlockToBinary(int digitSize, unsigned char* b);

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

#endif
