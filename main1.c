#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "fs.h"
#include "own_func.h"

/*
유의 사항
구현 해야할 함수들
FileSysInit, SetInodeBitmap, ResetInodeBitmap,SetBlockBitmap, ResetBlockBitmap, PutInode, GetInode, GetFreeInodeNum, GetFreeBlockNum
구현 해야할 파일들
fs.c 구현. fs.c에 임의의 헤더 파일(예,  temp.h 등)을 include 가능
fs.h, disk.c, disk.h 제공됨. 수정 불가(수정하면 감점)
Fs.h에는 구현 해야 할 function prototype 선언함.  
main.c에서 fs.h를 include해서 testcase가 제공되기 전에 각자 테스트함
향후 testcase를 포함하는 main.c을 제공할 계획

*/


void main(void)
{
	DevCreateDisk();
	FileSysInit();
	unsigned char* temp;
	
	temp = (unsigned char*)malloc(BLOCK_SIZE);
	
	Inode* nd = (Inode *)malloc(sizeof(Inode));

	nd->allocBlocks = 100000;

	printf("nd = %s\n", BlockToBinary(32, (unsigned char *)nd));
	printf("nd<<2 = %s\n", BlockToBinary(32, LeftShift((unsigned char *)nd, INODE_SIZE, 2)));
	PutInode(2, nd);
	printf("PutInode(2, nd);\n");
	nd = (Inode *)malloc(sizeof(Inode));
	printf("nd = %s\n", BlockToBinary(32, (unsigned char *)nd));
	GetInode(2, nd);
	printf("GetInode(2, nd);\n");
	printf("nd = %s\n", BlockToBinary(32, (unsigned char *)nd));

	for (int i = 0; i < 8; i++)
		SetInodeBitmap(i);

	printf("Freenum = %d\n", GetFreeInodeNum());
    
}