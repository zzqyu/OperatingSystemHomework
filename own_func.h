#ifndef __OWN_FUNC_H__
#define __OWN_FUNC_H__

#include <stdlib.h>
#include <string.h>

#define INODE_ITEM_NUM  32 /*Inode ����*/
#define FSI_NUM  0 /*File System Infomation block index num*/
#define DATA_REGION_FIRST  7 /* the first block no. of data region*/
#define INODE_SIZE  64 /* size(byte) of Inode*/

/*bit������ �迭�� �ٲ� ����
int digitSize: ���ڸ������� ����ϴ���.. , unsigned char* b: ����� ����*/
//unsigned char* BlockToBinary(int digitSize, unsigned char* b);

/*inodeSeq��° ��带 get, put
int inodeSeq: ����� ����, Inode* pInode: ����� Inode, char isGet: Get�Ϸ��� 1 Put�Ϸ��� 0*/
void GPInode(int inodeSeq, Inode* pInode, char isGet);

/*blkno��° ��Ͽ��� ù��°�� 0�� bit�ε�����ȣ ����
int blkno: ������ ��Ϲ�ȣ*/
int GetFreeNum(int blkno);

/* ��Ʈ�� ����� � �ε����� ������ 0�̳� 1�� ������.
int blkno: ������ ��Ϲ�ȣ, int bitmapIndex: ��Ʈ���� �ε�����ȣ, unsigned char bit: */
void SetBitmap(int blkno, int bitmapIndex, unsigned char bit);

/* Operation <<  : target << manyShiftBit
unsigned char* target: ������ ���, int targetByteSize: ������ ����� byte size, int manyShiftBit: ���Ʈ�� ����Ʈ����*/
unsigned char* LeftShift(unsigned char* target, int targetByteSize, int manyShiftBit);

/* Operation >>  : target >> manyShiftBit
unsigned char* target: ������ ���, int targetByteSize: ������ ����� byte size, int manyShiftBit: ���Ʈ�� ����Ʈ����*/
unsigned char* RightShift(unsigned char* target, int targetByteSize, int manyShiftBit);

/* Operation |=
lValue |= rValue , int valueByteSize : ����Ʈ ũ�� */
void OrEqual(unsigned char * lValue, unsigned char * rValue, int valueByteSize);

/* Operation &=
lValue &= rValue , int valueByteSize : ����Ʈ ũ�� */
void AndEqual(unsigned char * lValue, unsigned char * rValue, int valueByteSize);

/* Operation |
lValue | rValue , int valueByteSize : ����Ʈ ũ�� */
unsigned char* OperOr(unsigned char* value1, unsigned char* value2, int valueByteSize);

/* Operation &
lValue & rValue , int valueByteSize : ����Ʈ ũ�� */
unsigned char* OperAnd(unsigned char* value1, unsigned char* value2, int valueByteSize);

/* Operation (1 << fillBitSize) -1
(1 << fillBitSize) -1 , int valueByteSize : ����Ʈ ũ�� */
unsigned char* FillBit(int fillBitSize, int valueByteSize);

#endif
