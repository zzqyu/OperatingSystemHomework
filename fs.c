#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "fs.h"
#include "own_func.h"

/*FileSysinfo, inode bitmap, block bitmap, inode list를 0으로 채워서 초기화*/
void FileSysInit(void)
{
	DevCreateDisk();
    int dataRegionFirstIndex = INODELIST_BLK_FIRST + INODELIST_BLKS;

    //디스크 메모리 할당
    for(int i = 0; i<dataRegionFirstIndex; i++){
		unsigned char* block = (unsigned char*) malloc (BLOCK_SIZE);
        //non data region 0으로 초기화
        memset(block, 0, BLOCK_SIZE);
        //블록을 저장한다.
        DevWriteBlock(i, block);
    }

}
/*Inode 비트맵의 특정 순서의 비트를 1로 설정
int inodeno: Inode 순서*/
void SetInodeBitmap(int inodeno){
    SetBitmap(INODE_BITMAP_BLK_NUM,inodeno,(unsigned char)1);
}
/*Inode 비트맵의 특정 순서의 비트를 0로 설정
int inodeno: Inode 순서*/
void ResetInodeBitmap(int inodeno){
    SetBitmap(INODE_BITMAP_BLK_NUM,inodeno,(unsigned char)0);
}
/*Block 비트맵의 특정 순서의 비트를 1로 설정
int blkno: Block 순서*/
void SetBlockBitmap(int blkno){
    SetBitmap(BLOCK_BITMAP_BLK_NUM,blkno,(unsigned char)1);
}
/*Block 비트맵의 특정 순서의 비트를 0로 설정
int blkno: Block 순서*/
void ResetBlockBitmap(int blkno){
    SetBitmap(BLOCK_BITMAP_BLK_NUM,blkno,(unsigned char)0);
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
	GetFreeNum(INODE_BITMAP_BLK_NUM);
}

/*할당되지 않은 block 번호를 획득한다. */
int GetFreeBlockNum(void)
{
	return GetFreeNum(BLOCK_BITMAP_BLK_NUM);
}






