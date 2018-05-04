#include "Disk.h"
#include "fs.h"
#include "own_func.h"

void SetBitmap(int blkno, int bitmapIndex, unsigned char bit){
    //블록크기 만큼 메모리 할당
	unsigned char* tempBlock = (unsigned char*) malloc (BLOCK_SIZE);
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
	free(tempBlock);
}

int GetFreeNum(int blkno) {
	//블록크기 만큼 메모리 할당
	unsigned char* tempBlock = (unsigned char*)malloc(BLOCK_SIZE);
	unsigned char oneDigit = 0;
	//DevReadBlock함수로 읽어옴
	DevReadBlock(blkno, tempBlock);
	for (int i = 0; i < BLOCK_SIZE ; i++) {
		for (int j = 7; j >= 0; j--) { //reverse
			oneDigit = tempBlock[i] & (1<<j);
			if (oneDigit == (unsigned char)0) {
				free(tempBlock);
				return i * 8 + (7-j);
			}

			/*
			//for (int j = 0; j < 8; j++) { // original
			oneDigit = tempBlock[i] & 1;
			if (oneDigit == (unsigned char)0) {
				free(tempBlock);
				//return i * 8 + j;//첫째자리수가 0이면 i리턴 
				return i * 8 + j;//첫째자리수가 0이면 i리턴 
			}
				//1자리 >> 해준다.
			
			tempBlock[i] >>= 1;
			*/
		}
	}
	free(tempBlock);
	return -1;
}

void GPInode(int inodeSeq, Inode * pInode, char isGet)
{
	unsigned char* tempBlock = (unsigned char*)malloc(BLOCK_SIZE);
	int diskBlkno = inodeSeq / NUM_OF_INODE_PER_BLK + INODELIST_BLK_FIRST;

	//inodeSeq / 8 + 3;
	//메모리할당 + 3;
	int seqInBlock = inodeSeq % NUM_OF_INODE_PER_BLK;

	//해당 inode가 있는 블럭 읽어오기
	DevReadBlock(diskBlkno, tempBlock);
	//GET
	if (isGet == (char)1) {
		//Inode에 저장할 내용 뽑아내기
		//tempBlock = (tempBlock >> (8 * INODE_SIZE*seqInBlock)) & ((1<<(8 * INODE_SIZE*seqInBlock))-1);



		/*unsigned char* getNodeBit = (unsigned char*)malloc(BLOCK_SIZE);
		getNodeBit = FillBit(8 * sizeof(Inode), BLOCK_SIZE);//(1 << 8*INODE_SIZE*seqInBlock) - 1;

		tempBlock = OperAnd(RightShift(tempBlock, BLOCK_SIZE, 8 * sizeof(Inode)*seqInBlock), getNodeBit, sizeof(Inode));
		//Inode에 내용 복사
		memcpy((Inode*)pInode, tempBlock, sizeof(Inode));*/

		Inode* inodeArr = (Inode*)tempBlock;
		*pInode = inodeArr[seqInBlock];

		//free(getNodeBit);
	}
	//PUT
	else if (isGet == (char)0) {
		//해당 inode위치에 저장할 inode내용 넣기
		//tempBlock |= (*pInode << BLOCK_SIZE, 8*INODE_SIZE*seqInBlock);
		
		Inode* inodeArr = (Inode*)tempBlock;
		inodeArr[seqInBlock]  = *pInode ;
		DevWriteBlock(diskBlkno, (unsigned char*)inodeArr);
		/*OrEqual(tempBlock, LeftShift((unsigned char *)pInode, BLOCK_SIZE, 8* sizeof(Inode)*seqInBlock), BLOCK_SIZE);
		//DevWrite로 저장한다.
		DevWriteBlock(diskBlkno, tempBlock);*/
	}
	free(tempBlock);
	
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


//=======================HW2 ZONE =============================

DirEntry SetDirEntry(char* name, int inodeNum) {
	DirEntry answer;
	strcpy(answer.name, name);
	answer.inodeNum = inodeNum;
	return answer;
}

void CreateDirInBlock(int blockNo, int thisInodeNo, int parentInodeNo) {
	unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
	//비어있는 디렉토리 엔트리 리스트
	DirEntry* newDirEntry = (DirEntry*)block;
	//현재Dir의 엔트리 세팅
	newDirEntry[0] = SetDirEntry(".", thisInodeNo);
	//상위Dir의 엔트리 세팅, root인 경우는 세팅 안함
	if (parentInodeNo != -1)
		newDirEntry[1] = SetDirEntry("..", parentInodeNo);
	else newDirEntry[1] = SetDirEntry("", -1);
	newDirEntry[2] = SetDirEntry("", -1);
	newDirEntry[3] = SetDirEntry("", -1);
	//디스크에 기록
	block = (unsigned char*)newDirEntry;
	DevWriteBlock(blockNo, block);
	//블록비트맵 적용
	SetBlockBitmap(blockNo);
	//FSI 업데이트
	UpdateNumBlockFSI(1);
	free(block);
}
int  CreateIndirectBlock() {
	//인다이렉트 인덱스관리 블록 번호
	//int newIndirectBlockNum = GetDirBlockNo();
	int newIndirectBlockNum = GetFreeBlockNum();
	//블록 크기 공간 할당
	unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
	int* blockIndexs = (int *) block;
	/*  ====보류=======
	//인다이렉트블록에 들어갈 dir entry블록 no, 세팅
	//int newBlockNum = GetDirBlockNo();
	//AddDirEntry(newBlockNum);
	//blockIndexs[0] = newBlockNum;*/

	//blockIndexs 초기화
	for (int i = 0; i < BLOCK_SIZE / sizeof(int); i++) {
		blockIndexs[i] = -1;
	}
	//디스크에 기록
	DevWriteBlock(newIndirectBlockNum, block);
	//블록비트맵 적용
	SetBlockBitmap(newIndirectBlockNum);
	//FSI 업데이트 
	UpdateNumBlockFSI(1);
	free(block);
	return newIndirectBlockNum;
}

int AddDirEntry() {
	//새로 사용할 블록 넘버 얻기
	//int newBlockNum = GetDirBlockNo();
	int newBlockNum = GetFreeBlockNum();
	//디렉토리엔트리 초기화
	unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
	DirEntry* newDirEntry = (DirEntry*)block;
	for(int i = 0 ; i < BLOCK_SIZE/sizeof(DirEntry); i++ )
		newDirEntry[i] = SetDirEntry("", -1);

	//디스크에 기록
	block = (unsigned char*)newDirEntry;
	DevWriteBlock(newBlockNum, block);
	//블록비트맵 적용
	SetBlockBitmap(newBlockNum);
	//FSI 업데이트
	UpdateNumBlockFSI(1);
	free(block);
	return newBlockNum;
}

int UpdateDirInBlock(char* path, int newInodeNum) {
	//최상위 디렉 Inode를 획득해서 루트블록넘버를 획득
	//획득한 블록넘버로 해당블록의 Dir엔트리에 접근
	//해당 Dir엔트리 순차조회
	//접근할 디렉토리의 상위 디렉토리 엔트리를 볼수 있을 때까지 반복
	int pathLength = strlen(path+4); //경로의 길이
	char* tempPath = (char*)malloc(pathLength); //path 인자의 임시
	strcpy(tempPath, "");
	int inodeNum = GetDirInode(path, -1);
	
	if (isParentRoot(path)) 
		strcpy(tempPath, "ROOT");
	strcat(tempPath, path);
	
	Inode* inode = (Inode*)malloc(sizeof(Inode));
	GetInode(inodeNum, inode);
	// 생성할 파일/디렉 이름 만들기
	char lastName[MAX_NAME_LEN];
	char* temp = strtok(tempPath, "/");
	while ((temp = strtok(NULL, "/")) != NULL) {
		strcpy(lastName, temp);
	}

	free(tempPath);
	
	for (int i = 0; i < NUM_OF_DIRECT_BLK_PTR; i++) {
		//생성할 하위 디렉/파일 정보 기록 저장
		//다이렉트 블록 검사
		
		if (SetDirFileNameInBlock(inode->dirBlkPtr[i], lastName, newInodeNum)) {
			free(inode);
			return inodeNum;
		}
		//빈곳 없으면 다음 다이렉트 블록 검사
		//하기 전에 다음 다이렉트 블록이 할당이 안되어있으면
		//할당해줌
		if (i < NUM_OF_DIRECT_BLK_PTR-1 && inode->dirBlkPtr[i+1] == -1) {
			inode->dirBlkPtr[1] = AddDirEntry();
			PutInode(inodeNum, inode);
			/*inode = (Inode*)malloc(sizeof(Inode));
			GetInode(inodeNum, inode);*/
		}
		//빈곳이 없으면 인다이렉트 블록 검사
		//하기 전에 인다이렉트 블록이 할당이 안되어 있으면 
		//할당해줌
		else if (i == NUM_OF_DIRECT_BLK_PTR - 1 && inode->indirBlkPointer == -1) {
			inode->indirBlkPointer = CreateIndirectBlock();
			PutInode(inodeNum, inode);
			/*inode = (Inode*)malloc(sizeof(Inode));
			GetInode(inodeNum, inode);*/
		}

	}
	//인다이렉트 블록번호 리스트 가져오기
	unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
	DevReadBlock(inode->indirBlkPointer, block);
	int* blockIndexs = (int *)block;

	//빈곳 있는 
	for (int i = 0; i < BLOCK_SIZE / sizeof(int); i++) {
		if (blockIndexs[i] == -1) {
			blockIndexs[i] = AddDirEntry();
			DevWriteBlock(inode->indirBlkPointer, (unsigned char*)blockIndexs);
		}
		if (SetDirFileNameInBlock(blockIndexs[i], lastName, newInodeNum)) {
			free(block);
			free(inode);
			return inodeNum;
		}
	}
	free(block);
	free(inode);
	return -1;

	/*
	//생성할 하위 디렉/파일 정보 기록 저장
	//첫번째 다이렉트 블록 검사
	if (SetDirFileNameInBlock(inode->dirBlkPtr[0], lastName, inodeNum))
		return 1; 
	//빈곳 없으면 다음 다이렉트 블록 검사
	//하기 전에 다음 다이렉트 블록이 할당이 안되어있으면
	//할당해줌
	if (inode->dirBlkPtr[1] == -1) {
		inode->dirBlkPtr[1] = AddDirEntry();
		PutInode(inodeNum, inode);
	}
	//두번째 다이렉트 블록 검사
	if (SetDirFileNameInBlock(inode->dirBlkPtr[1], lastName, inodeNum))
		return 1;
	//빈곳이 없으면 인다이렉트 블록 검사
	//하기 전에 인다이렉트 블록이 할당이 안되어 있으면 
	//할당해줌
	if (inode->indirBlkPointer == -1) {
		inode->indirBlkPointer = CreateIndirectBlock();
		PutInode(inodeNum, inode);
	}*/
	
}
int SetDirFileNameInBlock(int blockNum, char* name, int inodeNum) {
	//정보를 기록할 엔트리가 있는 블록을 디스크에서 가져온다.
	int answer = 0;

	unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
	DevReadBlock(blockNum, block);
	DirEntry* dirEntryList = (DirEntry*)block;
	int i = 0;
	//비어있는 엔트리를 찾으면 정보를 기록한다.
	for (i = 0; i < BLOCK_SIZE / sizeof(DirEntry); i++) {
		if (dirEntryList[i].inodeNum == -1) {
			dirEntryList[i] = SetDirEntry(name, inodeNum);
			DevWriteBlock(blockNum, (unsigned char*)dirEntryList);
			SetInodeBitmap(inodeNum);
			UpdateNumInodeFSI(1);
			answer = 1;
			break;
		}
	}
	
	free(block);
	return answer;
}
int GetDirInode(char* path, int inodeNum) {
	int pathLength = strlen(path); //경로의 길이
	char* childPath = (char*)malloc(pathLength); //path 인자의 임시
	strcpy(childPath, path);

	
	char parentDirName[MAX_NAME_LEN]; // 상대 경로의 최상위 디렉토리 이름
	char thisDirName[MAX_NAME_LEN];  // 상대경로의 최상 다음(조회할) 디렉토리의 이름
	//char thisDirName[MAX_NAME_LEN];  // 상대경로의 최상 다음(조회할) 디렉토리의 이름
	//루트가 상위 디렉토리일 경우 문자열 분리를 위해 ROOT명시 위해 세팅함
	//FSI에서 루트의 InodeNo를 얻어 옴.
	if (path[0] == '/' && inodeNum == -1) {
		strcpy(childPath, "ROOT");
		strcat(childPath, path);
		unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
		DevReadBlock(FILESYS_INFO_BLOCK, block);
		FileSysInfo* fsi = (FileSysInfo*)block;
		inodeNum = fsi->rootInodeNum;
		free(block);
	}

	
	strcpy(parentDirName, strtok(childPath, "/"));
	strcpy(thisDirName, strtok(NULL, "/"));
	strcpy(childPath, thisDirName);

	char* temp = strtok(NULL, "\0");
	//탐색할 경로가 없는 경우
	if (temp == NULL){
		free(childPath);
		return inodeNum;
	}
	strcat(childPath, "/");
	strcat(childPath, temp);
	//검색결과가 없으면 -1리턴
	if ((inodeNum = GetInodeNumInAllDirEntry(thisDirName, inodeNum)) == -1) {
		free(childPath);
		return -1;
	}
	//조회할 경로가 남아있으면 하위의 상대경로로 계속 진행
	//free(childPath);
	GetDirInode(childPath, inodeNum);
	/*
	//상위디렉토리의 DirEntry에서 조회할 디렉토리의 Inode No.를 가져옴
	inodeNum = GetInodeNumInAllDirEntry(thisDirName, inodeNum);
	if (endFlag)  //조회할 경로가 없으면 Inode No. 리턴
		return inodeNum;
	else //조회할 경로가 남아있으면 하위의 상대경로로 계속 진행
		GetDirBlockNo(childPath,inodeNum);
	*/
}
int GetInodeNumInAllDirEntry(char* dirName, int inodeNum) {
	//조회할 디렉토리의 Inode 가져오기
	Inode* inode = (Inode*)malloc(sizeof(Inode));
	int answer = -1;
	GetInode(inodeNum, inode);

	//다이렉트 블록의 DirEntry 검사
	for (int i = 0; i < NUM_OF_DIRECT_BLK_PTR; i++) {
		//검사할 항목이 없으면 -1리턴
		if (inode->dirBlkPtr[i] == -1) return answer;
		//찾은 디렉토리의 InodeNo 리턴
		if ((answer = GetInodeNumInDirEntry(dirName, inode->dirBlkPtr[i])) != -1) return answer;
	}
	//다이렉트 블록 검사까지 마치면 Indirect Block 검사
	return GetInodeNumInAllIndirectDirEntry(dirName, inode->indirBlkPointer);
}
int GetInodeNumInAllIndirectDirEntry(char* dirName, int indirectBlockNum) {
	int answer = -1;
	// IndirectBlock이 없으면 검사할 대상이 없으니 -1 리턴
	if (indirectBlockNum == -1) return answer;
	//IndirectBlock index List 디스크에서 가져옴
	unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
	DevReadBlock(indirectBlockNum, block);
	int* blockIndexs = (int *)block;
	//순차적으로 모든 DirEntry조회
	for (int i = 0; i < BLOCK_SIZE/sizeof(int); i++) {
		if (blockIndexs[i] == -1) return answer;
		if ((answer = GetInodeNumInDirEntry(dirName, blockIndexs[i])) >= 0) break;
	}

	free(block);
	return answer;
}
int GetInodeNumInDirEntry(char* dirName, int blockNum) {
	unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
	DevReadBlock(blockNum, block);
	DirEntry* dirEntryList = (DirEntry*)block;
	int i = 0, answer = -1;
	for (i = 0; i < BLOCK_SIZE / sizeof(DirEntry);i++) {
		if (strcmp(dirEntryList[i].name, dirName)==0) {
			answer = dirEntryList[i].inodeNum;
			break;
		}
		else if (dirEntryList[i].inodeNum == -1) break;
	}
	free(block);

	return answer;
}

void CreateDirInode(int blockNo, int thisInodeNo){
	Inode* newInode = (Inode*)malloc(sizeof(Inode));
	newInode->dirBlkPtr[0] = blockNo;
	newInode->dirBlkPtr[1] = -1;
	newInode->indirBlkPointer = -1;
	
	PutInode(thisInodeNo, newInode);
	
	free(newInode);
}
void InitFileSysInfo() {
	unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
	DevReadBlock(FILESYS_INFO_BLOCK, block);
	FileSysInfo* fsi = (FileSysInfo*)block;
	fsi->blocks = BLOCK_SIZE * NUM_BIT_OF_BYTE;
	fsi->rootInodeNum = 0;
	fsi->diskCapacity = FS_DISK_CAPACITY;
	fsi->numAllocBlocks = 0;
	
	fsi->numAllocInodes = 0;
	fsi->blockBitmapBlock = BLOCK_BITMAP_BLK_NUM;
	fsi->inodeBitmapBlock = INODE_BITMAP_BLK_NUM;
	fsi->inodeListBlock = INODELIST_BLK_FIRST;
	fsi->dataRegionBlock = INODELIST_BLK_FIRST + INODELIST_BLKS;

	fsi->numFreeBlocks = (fsi->blocks) - (fsi->dataRegionBlock);
	block = (unsigned char*)fsi;
	DevWriteBlock(FILESYS_INFO_BLOCK, block);
	free(block);
}

void UpdateNumBlockFSI(int isAdd) {
	int addNum = -1 + (2 * isAdd);
	unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
	DevReadBlock(FILESYS_INFO_BLOCK, block);
	FileSysInfo* fsi = (FileSysInfo*)block;
	fsi->numAllocBlocks+= addNum;
	fsi->numFreeBlocks-= addNum;
	block = (unsigned char*) fsi;
	DevWriteBlock(FILESYS_INFO_BLOCK, block);
	free(block);
}
void UpdateNumInodeFSI(int isAdd) {
	int addNum = -1 + (2 * isAdd);
	unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
	DevReadBlock(FILESYS_INFO_BLOCK, block);
	FileSysInfo* fsi = (FileSysInfo*)block;
	fsi->numAllocInodes += addNum;
	block = (unsigned char*)fsi;
	DevWriteBlock(FILESYS_INFO_BLOCK, block);
	free(block);
}
int isParentRoot(char* path) {
	int count = 0;
	for (int i = 0; i < strlen(path); i++) {
		if (path[i] == '/') count++;
	}
	return (count == 1) ? 1 : 0;
}



void PrintInode(int inodeNum) {
	Inode* inode = (Inode*)malloc(sizeof(Inode));
	GetInode(inodeNum, inode);

	printf("==========================\n");
	printf("inode[%d].dirBlkPtr[0] = %d\n", inodeNum, inode->dirBlkPtr[0]);
	printf("inode[%d].dirBlkPtr[1] = %d\n", inodeNum, inode->dirBlkPtr[1]);
	printf("inode[%d].indirBlkPointer = %d\n", inodeNum, inode->indirBlkPointer);
	free(inode);
}
void PrintDirEntryList(int blockNum){
	unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
	DevReadBlock(blockNum, block);
	DirEntry* dEntryList = (DirEntry*)block;
	printf("==========================\n");

	for (int i = 0; i < 4; i++) {
		printf("Block %d Entry[%d] : %s, %d \n", blockNum, i, dEntryList[i].name, dEntryList[i].inodeNum);
	}
	free(block);

}
void PrintIndirectIndexList(int blockNum) {
	//블록 크기 공간 할당
	unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
	DevReadBlock(blockNum, block);
	int* blockIndexs = (int *)block;
	printf("==========================\n");

	for (int i = 0; i < BLOCK_SIZE / sizeof(int); i++) {
		printf("Block %d Index[%d] : %d \n", blockNum, i, blockIndexs[i]);
	}
	free(block);
}
