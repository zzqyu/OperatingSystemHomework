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

//#define INODE_NUM_OF_BLOCK 4 /*�� ��Ͽ� ���� inode����*/
//#define INODE_ITEM_NUM  INODE_NUM_OF_BLOCK*INODELIST_BLKS /*Inode ����*/

/*bit������ �迭�� �ٲ� ����
int digitSize: ���ڸ������� ����ϴ���.. , unsigned char* b: ����� ����*/
unsigned char* BlockToBinary(int digitSize, unsigned char* b);

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


//=======================HW2 ZONE =============================

/*	DirEntry �׸��� �����ϴ� �Լ�
	char* name :: ������ �׸�(���丮/����) �̸�
	int inodeNum :: ������ �׸��� Inode ��ȣ
	return ������ DriEntryItem  */
DirEntry SetDirEntry(char* name, int inodeNum);
/*	������ ���丮�� ��Ʈ�� ����Ʈ�� �ʱ�ȭ
	int blockNo :: ���丮 ��Ʈ���� ���� Block No.
	int thisInodeNo :: �����ϴ� ���丮�� Inode No.
	int parentInodeNo :: ���� ���丮�� Inode No.
	return void */
void CreateDirInBlock(int blockNo, int thisInodeNo, int parentInodeNo);
/*	Indirect Block ���� �� �ʱ�ȭ
	return ������ Indirect Block�� Block No.*/
int CreateIndirectBlock();
/*	Second Direct Block ���� �� �ʱ�ȭ
	return ������ Seocond Direct Block�� Block No.*/
int AddDirEntry();
/*	������ �׸�(���丮/����)�� ���� ���丮�� ��Ʈ���� 
	��Ʈ���� ���������� ã�Ƽ� �̸�, Inode��ȣ���
	char* path :: ������ �׸��� ������
	int newInodeNum :: ������ �׸� ������ Inode No.
	return 0�̻� :: ���� ���丮�� Inode��ȣ
	return -1 :: ����
*/
int UpdateDirInBlock(char* path, int newInodeNum);
/*	�ش� ����� Dir Entry List�� �� ���� ������
	������ �̸�, Inode��ȣ���
	int blockNum :: ��ȸ�� Block No.
	char* name :: �׸��� �̸�
	int inodeNum :: Inode No.
	return 1 :: ����
	return 0 :: ���� */
int SetDirFileNameInBlock(int blockNum, char* name, int inodeNum);
/*	�߰��� �׸��� �������丮�� Inode No.�� ������ �Լ�
	char* path :: �߰� ��û�� ������
	int inodeNum :: (���� ���� �� -1) ��ȸ�� ���丮�� Inode No.
	return �߰��� �׸��� �������丮�� Inode No.
	return -1 :: ����*/
int GetDirInode(char* path, int inodeNum);
/*	���丮�� ��� ��Ʈ�� ����Ʈ���� �̸��� ��ġ�ϴ� InodeNo ����
	char* dirName :: �˻��� ���丮 ����
	int inodeNum :: �˻��ϴ� ���丮�� Inode No.
	return ���̸��� ��ġ�ϴ� InodeNo
	return -1 :: ����*/
int GetInodeNumInAllDirEntry(char* dirName, int inodeNum);
/*	���丮�� ��� Indirect ��Ʈ������Ʈ���� �̸��� ��ġ�ϴ� InodeNo ����
	char* dirName :: �˻��� ���丮 ����
	int indirectBlockNum :: �˻��ϴ� �δ��̷�Ʈ index block�� Inode No.
	return ���̸��� ��ġ�ϴ� InodeNo
	return -1 :: ����*/
int GetInodeNumInAllIndirectDirEntry(char* dirName, int indirectBlockNum);
/*	�� ����� ��� ��Ʈ������Ʈ���� �̸��� ��ġ�ϴ� InodeNo ����
	char* dirName :: �˻��� ���丮 ����
	int inodeNum :: �˻��ϴ� ���丮�� Inode No.
	return ���̸��� ��ġ�ϴ� InodeNo
	return -1 :: ����*/
int GetInodeNumInDirEntry(char* dirName, int blockNum);
/* Inode ���� �� �ʱ�ȭ
	int blockNo :: ù��° DirEntry�� �ִ� Block�� No.
	int thisInodeNo :: �����ϴ� Inode ��ȣ*/
void CreateDirInode(int blockNo, int thisInodeNo);
/* FSI �ʱ�ȭ*/
void InitFileSysInfo();
/* FSI�� numAllocBlocks, numFreeBlocks ������Ʈ
	int isAdd :: 1�̸� �Ҵ� ��� 1����, 0�̸� 1����*/
void UpdateNumBlockFSI(int isAdd);
/* FSI�� numAllocInodes ������Ʈ
int isAdd :: 1�̸� �Ҵ� Inode 1����, 0�̸� 1����*/
void UpdateNumInodeFSI(int isAdd);
/*�� ����� ���� ���丮�� ��Ʈ���� Ȯ��*/
int isParentRoot(char* path);

void PrintInode(int inodeNum);
void PrintDirEntryList(int blockNum);
void PrintIndirectIndexList(int blockNum);

#endif
