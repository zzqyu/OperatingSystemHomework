#include <stdio.h>
#include <stdlib.h>
#include "fs.h"
#include "Disk.h"
#include "own_func.h"

FileSysInfo* pFileSysInfo = NULL;

/**/
void		Mount(MountType type)
{
	if (type == MT_TYPE_FORMAT) {
		//Block 512�� ��� �ʱ�ȭ
		FileSysInit();
		//Block Bitmap 0~18bit 1�� �ʱ�ȭ
		unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
		//1110 0000
		block[0] = (unsigned char)0xff;
		block[1] = (unsigned char)0xff;
		block[2] = (unsigned char)0xe0;
		DevWriteBlock(BLOCK_BITMAP_BLK_NUM, block);
		
		//free(block);

		//FileSysInfo �ʱ�ȭ
		InitFileSysInfo();
		//��Ʈ���丮 ����
		int newBlockNum = GetFreeBlockNum();
		int newInodeNum = GetFreeInodeNum();
		//��Ʈ ���丮�� DirEntry ����Ʈ ����/�ʱ�ȭ
		CreateDirInBlock(newBlockNum, newInodeNum, -1);
		SetInodeBitmap(newInodeNum);
		UpdateNumInodeFSI(1);
		//��Ʈ ���丮�� Inode ����/�ʱ�ȭ
		CreateDirInode(newBlockNum, newInodeNum);

		DevReadBlock(3, block);
	}
}


void		Unmount(void)
{

}

