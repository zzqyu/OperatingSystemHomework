#include "disk.h"
#include "fs.h"
#include "own_func.h"

void SetBitmap(int blkno, int bitmapIndex, unsigned char bit){
    //블록크기 만큼 메모리 할당
    char* tempBlock = (char*) malloc (BLOCK_SIZE);
    //DevReadBlock함수로 읽어옴
    DevReadBlock(blkno, tempBlock);
    //원하는 비트 index자리수를 1or0로 바꿔줌
	
	if (bit == (unsigned char)1)
		//tempBlock[bitmapIndex / 8] |= (1 << (bitmapIndex%8)); //original
		tempBlock[bitmapIndex / 8] |= (1 << (7-bitmapIndex%8)); //reverse
	else if (bit == (unsigned char)0)
		//tempBlock[bitmapIndex / 8] &= ~(1 << ( bitmapIndex % 8)); //original
		tempBlock[bitmapIndex / 8] &= ~(1 << (7 - bitmapIndex % 8)); //reverse

    DevWriteBlock(blkno, tempBlock);
}

int GetFreeNum(int blkno) {
	//블록크기 만큼 메모리 할당
	unsigned char* tempBlock = (unsigned char*)malloc(BLOCK_SIZE);
	unsigned char oneDigit = 0;
	//DevReadBlock함수로 읽어옴
	DevReadBlock(blkno, tempBlock);
	for (int i = 0; i < BLOCK_SIZE ; i++) {
		for (int j = 7; j >= 0; j--) { //reverse
		//for (int j = 0; j < 8; j++) { // original
			oneDigit = tempBlock[i] & 1;//첫째자리수
			if (oneDigit == (unsigned char)0) return i*8+j;//첫째자리수가 0이면 i리턴
													   //1자리 >> 해준다.
			tempBlock[i] >>= 1;
		}
	}
	return -1;
}

void GPInode(int inodeSeq, Inode * pInode, char isGet)
{
	//메모리할당
	unsigned char* tempBlock = (unsigned char*)malloc(BLOCK_SIZE);
	int diskBlkno = inodeSeq / 8 + 3;
	int seqInBlock = inodeSeq % 8;
	//해당 inode가 있는 블럭 읽어오기
	DevReadBlock(diskBlkno, tempBlock);
	//GET
	if (isGet == (char)1) {
		//Inode에 저장할 내용 뽑아내기
		//tempBlock = (tempBlock >> (8 * INODE_SIZE*seqInBlock)) & ((1<<(8 * INODE_SIZE*seqInBlock))-1);
		unsigned char* getNodeBit = (unsigned char*)malloc(BLOCK_SIZE);
		getNodeBit = FillBit(8 * INODE_SIZE*seqInBlock, BLOCK_SIZE);//(1 << 8*INODE_SIZE*seqInBlock) - 1;
		tempBlock = OperAnd(RightShift(tempBlock, BLOCK_SIZE, 8 * INODE_SIZE*seqInBlock), getNodeBit, BLOCK_SIZE);
		//Inode에 내용 복사
		memcpy((unsigned char*)pInode, tempBlock, INODE_SIZE);
	}
	//PUT
	else if (isGet == (char)0) {
		//해당 inode위치에 저장할 inode내용 넣기
		//tempBlock |= (*pInode << BLOCK_SIZE, 8*INODE_SIZE*seqInBlock);
		OrEqual(tempBlock, LeftShift((unsigned char *)pInode, BLOCK_SIZE, 8*INODE_SIZE*seqInBlock), BLOCK_SIZE);
		//DevWrite로 저장한다.
		DevWriteBlock(diskBlkno, tempBlock);
	}
}
/*
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
}*/
unsigned char* LeftShift(unsigned char* target, int targetByteSize, int manyShiftBit ) {
	if (manyShiftBit == 0)
		return target;		

	unsigned char* answer = (unsigned char*)malloc(targetByteSize);
	int targetBitSize = targetByteSize * 8;
	int i = 0, j = 0;

	memset(answer, 0, targetByteSize);
	i = manyShiftBit / 8;
	answer[i++] = target[0]<< (manyShiftBit % 8);
	if (i * 8 >= targetBitSize - 8) return answer;
	for (j = 0; j < (targetBitSize - manyShiftBit) / 8; j++) {
		answer[i++] = (target[j] >> (8 - (manyShiftBit % 8)))| (target[j+1] << (manyShiftBit % 8));
		if (i * 8 == targetBitSize-8) break;
	}
	answer[i] = target[j] >> (8 - (manyShiftBit % 8));
	return answer;
}
unsigned char* RightShift(unsigned char* target, int targetByteSize, int manyShiftBit) {
	unsigned char* answer = (unsigned char*)malloc(targetByteSize);
	int targetBitSize = targetByteSize * 8;
	int i = 0, j = 0;

	memset(answer, 0, targetByteSize);
	if (manyShiftBit == 0)
		return target;
	else if (targetBitSize <= manyShiftBit)
		return answer;


	for (j = manyShiftBit / 8 ; j < targetBitSize / 8 - 1; j++) {
		answer[i++] = (target[j] >> (manyShiftBit % 8)) | (target[j + 1] << (8 - manyShiftBit % 8));
		if (i * 8 == targetBitSize) break;
	}
	answer[i] = target[j] >> (manyShiftBit % 8);
	return answer;

}

void OrEqual(unsigned char * lValue, unsigned char * rValue, int valueByteSize)
{
	for (int i = 0; i < valueByteSize; i++) {
		lValue[i] |= rValue[i];
	}
}

void AndEqual(unsigned char * lValue, unsigned char * rValue, int valueByteSize)
{
	for (int i = 0; i < valueByteSize; i++) {
		lValue[i] &= rValue[i];
	}
}

unsigned char* OperOr(unsigned char* value1, unsigned char* value2, int valueByteSize) {
	unsigned char* answer = (unsigned char*)malloc(valueByteSize);
	for (int i = 0; i < valueByteSize; i++) {
		answer[i] = value1[i] | value2[i];
	}
	return answer;
}
unsigned char* OperAnd(unsigned char* value1, unsigned char* value2, int valueByteSize) {
	unsigned char* answer = (unsigned char*)malloc(valueByteSize);
	for (int i = 0; i < valueByteSize; i++) {
		answer[i] = value1[i] & value2[i];
	}
	return answer;
}
unsigned char* FillBit(int fillBitSize, int valueByteSize) {
	unsigned char* answer = (unsigned char*)malloc(valueByteSize);
	int i = 0;
	memset(answer, 0, valueByteSize);
	for (i = 0; i < (fillBitSize -1) / 8; i++) {
		answer[i] = ~0;
	}
	if(fillBitSize % 8 == 0)
		answer[i] = ~0;
	else
		answer[i] = (1 << (fillBitSize % 8)) - 1;

	return answer;
}
