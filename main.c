#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "fs.h"
#define INODE_SIZE 64


char IsDigitOne(unsigned char* block, int bitIndex);
unsigned char* BlockToBinary(int digitSize, unsigned char* b);

void main(void)
{
	DevCreateDisk();
	FileSysInit();
	unsigned char* temp;
	
	temp = (unsigned char*)malloc(BLOCK_SIZE);
	
	Inode* nd = (Inode *)malloc(sizeof(Inode));

	nd->allocBlocks = 100000;

	printf("INODE nd = %s\n", BlockToBinary(32, (unsigned char *)nd));
	printf("IsDigitOne(5) = %c\n", IsDigitOne((unsigned char *)nd, 5));
	printf("IsDigitOne(6) = %c\n", IsDigitOne((unsigned char *)nd, 6));
	printf("IsDigitOne(7) = %c\n", IsDigitOne((unsigned char *)nd, 7));
	printf("IsDigitOne(8) = %c\n", IsDigitOne((unsigned char *)nd, 8));
	PutInode(2, nd);
	printf("PutInode(2, nd);\n");
	nd = (Inode *)malloc(sizeof(Inode));
	printf("(초기화)INODE nd = %s\n", BlockToBinary(32, (unsigned char *)nd));
	temp = (unsigned char*)malloc(BLOCK_SIZE);
	DevReadBlock(3, temp);
	printf("IsDigitOne(5+%d) = %c\n", 8 * INODE_SIZE * 2, IsDigitOne(temp, 5 + 8 * INODE_SIZE * 2));
	printf("IsDigitOne(6+%d) = %c\n", 8 * INODE_SIZE * 2, IsDigitOne(temp, 6 + 8 * INODE_SIZE * 2));
	printf("IsDigitOne(7+%d) = %c\n", 8 * INODE_SIZE * 2, IsDigitOne(temp, 7 + 8 * INODE_SIZE * 2));
	printf("IsDigitOne(8+%d) = %c\n", 8 * INODE_SIZE * 2, IsDigitOne(temp, 8 + 8 * INODE_SIZE * 2));
	GetInode(2, nd);
	printf("GetInode(2, nd);\n");
	printf("INODE nd = %s\n", BlockToBinary(32, (unsigned char *)nd));
	printf("IsDigitOne(5) = %c\n", IsDigitOne((unsigned char *)nd, 5));
	printf("IsDigitOne(6) = %c\n", IsDigitOne((unsigned char *)nd, 6));
	printf("IsDigitOne(7) = %c\n", IsDigitOne((unsigned char *)nd, 7));
	printf("IsDigitOne(8) = %c\n", IsDigitOne((unsigned char *)nd, 8));
	temp = (unsigned char*)malloc(BLOCK_SIZE);
	for (int i = 0; i < 511; i++) {
		temp[i] = 0xff;
	}
	temp[511] = 0x7f;
	printf("for (int i = 0; i < 511; i++) {\n\ttemp[i] = 0xff;\n}\ntemp[511] = 0x7f;\nDevWriteBlock(INODE_BITMAP_BLK_NUM, temp);\n");
	DevWriteBlock(INODE_BITMAP_BLK_NUM, temp);
	printf("GetFreeInodeNum() = %d\n", GetFreeInodeNum());
 
}
char IsDigitOne(unsigned char* block, int bitIndex) {
	unsigned char resultBit = 0;
	unsigned char* temp = (unsigned char*)malloc(BLOCK_SIZE);
	memcpy(temp, block, BLOCK_SIZE);
	resultBit = (temp[bitIndex / 8] >> (bitIndex % 8)) & 1;
	return (resultBit == 1) ? '1' : '0';
}
unsigned char* BlockToBinary(int digitSize, unsigned char* b) {
	unsigned char* s = (unsigned char*)malloc(digitSize + 1);
	unsigned char* temp = (unsigned char*)malloc(BLOCK_SIZE);
	int i = 0;
	memcpy(temp, b, BLOCK_SIZE);
	s[digitSize] = '\0';
	for (int count = digitSize - 1; count >= 0; count--) {
		s[i++] = (((temp[count / 8] >> (count % 8)) & 1) == (unsigned char)0) ? '0' : '1';
	}
	return s;
}