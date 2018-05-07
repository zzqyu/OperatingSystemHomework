#include <stdio.h>
#include <stdlib.h>
#include "fs.h"
#include "disk.h"
#include "own_func.h"

FileSysInfo* pFileSysInfo = NULL;

/**/
void		Mount(MountType type)
{
	if (type == MT_TYPE_FORMAT) {
		//Block 512개 모두 초기화
		FileSysInit();
		//Block Bitmap 0~18bit 1로 초기화
		unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
		//1110 0000
		block[0] = (unsigned char)0xff;
		block[1] = (unsigned char)0xff;
		block[2] = (unsigned char)0xe0;
		DevWriteBlock(BLOCK_BITMAP_BLOCK_NUM, block);
		
		//free(block);

		//FileSysInfo 초기화
		InitFileSysInfo();
		//루트디렉토리 생성
		int newBlockNum = GetFreeBlockNum();
		int newInodeNum = GetFreeInodeNum();
		//루트 디렉토리의 DirEntry 리스트 생성/초기화
		CreateDirInBlock(newBlockNum, newInodeNum, -1);
		SetInodeBitmap(newInodeNum);
		UpdateNumInodeFSI(1);
		//루트 디렉토리의 Inode 생성/초기화
		CreateDirInode(newBlockNum, newInodeNum, 0);

		DevReadBlock(3, block);
	}
}


void		Unmount(void)
{

}

