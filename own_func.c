#include "disk.h"
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
	Inode* inodeArr = (Inode*)malloc(BLOCK_SIZE);
	int diskBlkno = inodeSeq / NUM_OF_INODE_PER_BLOCK + INODELIST_BLOCK_FIRST;

	//inodeSeq / 8 + 3;
	//메모리할당 + 3;
	int seqInBlock = inodeSeq % NUM_OF_INODE_PER_BLOCK;

	//해당 inode가 있는 블럭 읽어오기
	DevReadBlock(diskBlkno, (char*)inodeArr);
	//GET
	if (isGet == (char)1) {
		//Inode에 저장할 내용 뽑아내기
		//tempBlock = (tempBlock >> (8 * INODE_SIZE*seqInBlock)) & ((1<<(8 * INODE_SIZE*seqInBlock))-1);



		/*unsigned char* getNodeBit = (unsigned char*)malloc(BLOCK_SIZE);
		getNodeBit = FillBit(8 * sizeof(Inode), BLOCK_SIZE);//(1 << 8*INODE_SIZE*seqInBlock) - 1;

		tempBlock = OperAnd(RightShift(tempBlock, BLOCK_SIZE, 8 * sizeof(Inode)*seqInBlock), getNodeBit, sizeof(Inode));
		//Inode에 내용 복사
		memcpy((Inode*)pInode, tempBlock, sizeof(Inode));*/

		*pInode = inodeArr[seqInBlock];

		//free(getNodeBit);
	}
	//PUT
	else if (isGet == (char)0) {
		//해당 inode위치에 저장할 inode내용 넣기
		//tempBlock |= (*pInode << BLOCK_SIZE, 8*INODE_SIZE*seqInBlock);
		
		inodeArr[seqInBlock]  = *pInode ;
		DevWriteBlock(diskBlkno, (char*)inodeArr);

		/*OrEqual(tempBlock, LeftShift((unsigned char *)pInode, BLOCK_SIZE, 8* sizeof(Inode)*seqInBlock), BLOCK_SIZE);
		//DevWrite로 저장한다.
		DevWriteBlock(diskBlkno, tempBlock);*/
	}
	free(inodeArr);
	
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
FileDesc SetFileDesc(int bUsed, int offset, int inodeNum) {
	FileDesc answer;
	answer.bUsed = bUsed;
	answer.fileOffset = offset;
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

int UpdateDirInBlock(char* path, int newInodeNum, int isUpdate) {
	//최상위 디렉 Inode를 획득해서 루트블록넘버를 획득
	//획득한 블록넘버로 해당블록의 Dir엔트리에 접근
	//해당 Dir엔트리 순차조회
	//접근할 디렉토리의 상위 디렉토리 엔트리를 볼수 있을 때까지 반복
	int endFlag = 0;
	int pathLength = strlen(path+4); //경로의 길이
	char* tempPath = (char*)malloc(pathLength); //path 인자의 임시
	strcpy(tempPath, "");
	int inodeNum = GetDirInode(path, -1);
	//항목 추가 결과
	int setResult = -1;
	printf("[UpdateDirInBlock]inodeNum = %d\n", inodeNum);
	if (isParentRoot(path))
		strcpy(tempPath, "ROOT");
	strcat(tempPath, path);

	Inode* inode = (Inode*)malloc(sizeof(Inode));
	GetInode(inodeNum, inode);
	// 생성할 파일/디렉 이름 만들기
	char lastName[MAX_NAME_LEN] = "";
	strcpy(lastName, GetItemName(tempPath));
	free(tempPath);

	for (int i = 0; i < NUM_OF_DIRECT_BLOCK_PTR; i++) {
		//생성할 하위 디렉/파일 정보 기록 저장
		//다이렉트 블록 검사
		
		if ( (setResult =SetDirFileNameInBlock(inode->dirBlockPtr[i], lastName, newInodeNum, isUpdate)) == newInodeNum ) {
			setResult = inodeNum;
			endFlag = 1;
			break;
		}
		else if (setResult >= 10000) {
			endFlag = 1;
			break;
		}
		//빈곳 없으면 다음 다이렉트 블록 검사
		//하기 전에 다음 다이렉트 블록이 할당이 안되어있으면
		//할당해줌
		if (isUpdate == 1 && (i < NUM_OF_DIRECT_BLOCK_PTR-1 && inode->dirBlockPtr[i+1] == -1)) {
			inode->dirBlockPtr[1] = AddDirEntry();
			PutInode(inodeNum, inode);
			/*inode = (Inode*)malloc(sizeof(Inode));
			GetInode(inodeNum, inode);*/
		}
		//빈곳이 없으면 인다이렉트 블록 검사
		//하기 전에 인다이렉트 블록이 할당이 안되어 있으면 
		//할당해줌
		else if (isUpdate == 1 && (i == NUM_OF_DIRECT_BLOCK_PTR - 1 && inode->indirBlockPtr == -1)) {
			inode->indirBlockPtr = CreateIndirectBlock();
			PutInode(inodeNum, inode);
			/*inode = (Inode*)malloc(sizeof(Inode));
			GetInode(inodeNum, inode);*/
		}

	}
	if (endFlag) {
		free(inode);
		return setResult;
	}
	//인다이렉트 블록번호 리스트 가져오기
	unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
	DevReadBlock(inode->indirBlockPtr, block);
	int* blockIndexs = (int *)block;

	//빈곳 있는 
	for (int i = 0; i < BLOCK_SIZE / sizeof(int); i++) {
		if (isUpdate == 1  && blockIndexs[i] == -1) {
			blockIndexs[i] = AddDirEntry();
			DevWriteBlock(inode->indirBlockPtr, (unsigned char*)blockIndexs);
		}
		if ((setResult = SetDirFileNameInBlock(blockIndexs[i], lastName, newInodeNum, isUpdate)) == newInodeNum) {
			setResult = inodeNum;
			break;
		}
		else if (setResult >= 10000) {
			break;
		}
	}
	free(block);
	free(inode);
	return setResult;
	
}
int SetDirFileNameInBlock(int blockNum, char* name, int inodeNum, int isUpdate) {
	//정보를 기록할 엔트리가 있는 블록을 디스크에서 가져온다.
	int answer = -1;
	
	unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
	DevReadBlock(blockNum, block);
	
	DirEntry* dirEntryList = (DirEntry*)block;
	int i = 0;
	//비어있는 엔트리를 찾으면 정보를 기록한다.
	for (i = 0; i < BLOCK_SIZE / sizeof(DirEntry); i++) {
		if (isUpdate ==1 && dirEntryList[i].inodeNum == -1) {
			dirEntryList[i] = SetDirEntry(name, inodeNum);
			DevWriteBlock(blockNum, (unsigned char*)dirEntryList);
			SetInodeBitmap(inodeNum);
			//UpdateDirInode(inodeNum, 1);
			UpdateNumInodeFSI(1);
			answer = inodeNum;


			break;
		}
		else if (strcmp(name, dirEntryList[i].name)==0) {
			answer = 10000 + dirEntryList[i].inodeNum;
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
	
}
int GetInodeNumInAllDirEntry(char* dirName, int inodeNum) {
	//조회할 디렉토리의 Inode 가져오기
	Inode* inode = (Inode*)malloc(sizeof(Inode));
	int answer = -1;
	GetInode(inodeNum, inode);

	//다이렉트 블록의 DirEntry 검사
	for (int i = 0; i < NUM_OF_DIRECT_BLOCK_PTR; i++) {
		//검사할 항목이 없으면 -1리턴
		if (inode->dirBlockPtr[i] == -1) return answer;
		//찾은 디렉토리의 InodeNo 리턴
		if ((answer = GetInodeNumInDirEntry(dirName, inode->dirBlockPtr[i])) != -1) return answer;
	}
	//다이렉트 블록 검사까지 마치면 Indirect Block 검사
	return GetInodeNumInAllIndirectDirEntry(dirName, inode->indirBlockPtr);
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

void CreateDirInode(int blockNo, int thisInodeNo, int isNotRoot){
	Inode* newInode = (Inode*)malloc(sizeof(Inode));
	newInode->size = 16 * (isNotRoot + 1);
	newInode->type = FILE_TYPE_DIR;
	newInode->dirBlockPtr[0] = blockNo;
	newInode->dirBlockPtr[1] = -1;
	newInode->indirBlockPtr = -1;
	
	PutInode(thisInodeNo, newInode);
	
	free(newInode);
}
void CreateFileInode(int thisInodeNo) {
	Inode* newInode = (Inode*)malloc(sizeof(Inode));
	newInode->size = 0;
	newInode->type = FILE_TYPE_FILE;
	newInode->dirBlockPtr[0] = -1;
	newInode->dirBlockPtr[1] = -1;
	newInode->indirBlockPtr = -1;

	PutInode(thisInodeNo, newInode);

	free(newInode);
}
void UpdateDirInode(int inodeNum, int isAdd) {
	UpdateSizeOfInode(inodeNum, -16 + (16 * 2 * isAdd));
}
void UpdateSizeOfInode(int inodeNum, int addSize) {
	Inode* inode = (Inode*)malloc(sizeof(Inode));
	GetInode(inodeNum, inode);
	inode->size += addSize;
	PutInode(inodeNum, inode);
	free(inode);
}

void LoseFileBlock(int offset, int inodeNum) {
	Inode* inode = (Inode*)malloc(sizeof(Inode)); //작업하는 Inode
	GetInode(inodeNum, inode); //Disk에서 Inode로드
							   // file사이즈가 offset보다 클 경우는 블록을 버릴 이유가 없다.
	if (inode->size <= offset) return;

	int startBlockSeq = offset / BLOCK_SIZE; //내용 삭제 시작할 Inode의 블록 순서
											 //첫번째 삭제 블록에서 삭제 시작할 offset
	int startOffsetInFirstBlock = offset % BLOCK_SIZE;
	int endBlockSeq = inode->size / BLOCK_SIZE; //내용삭제하는 Inode의 블록마지막순서


	int* indirectBlockNumList = NULL; //인다이렉트블록인덱스 리스트
									  //인다이렉트블록이 존재하는 경우 리스트로드
	if (inode->indirBlockPtr != -1) {
		indirectBlockNumList = (int*)malloc(BLOCK_SIZE);
		DevReadBlock(inode->indirBlockPtr, (char*)indirectBlockNumList);
	}
	//블록내용 버리기
	for (int i = startBlockSeq; i <= endBlockSeq; i++) {
		int blockNum = -1; //작업하는 블록번호
		int startOffsetInBlock = 0; //삭제시작하는 블록의 바이트순서
		int isDirect = 0; //다이렉트블록인가요?
						  //다이렉트블록인 경우 
		if (i < NUM_OF_DIRECT_BLOCK_PTR) {
			blockNum = inode->dirBlockPtr[i];
			isDirect = 1;
		}
		//인다이렉트 블록인경우
		else
			blockNum = indirectBlockNumList[i - NUM_OF_DIRECT_BLOCK_PTR];

		//삭제 시작할 블록 내의 바이트 정하기
		//작업할 블록이 없는 경우
		if (blockNum == -1)
			startOffsetInBlock = -1;
		//작업하는 블록이 첫 블록이면
		else if ((blockNum != -1) && i == startBlockSeq)
			startOffsetInBlock = startOffsetInFirstBlock;

		//작업할 블록이 없는 경우 끝
		if (startOffsetInBlock == -1) break;
		//작업할 블록이 있는 경우 블록을 초기화 하자
		else {
			unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
			if (startOffsetInBlock > 0) {
				DevReadBlock(blockNum, block);
				for (int j = startOffsetInBlock; j < BLOCK_SIZE; j++)
					block[j] = (unsigned char)0;
			}
			//첫블록이 아닌 경우 Inode에서 가리키는 블록을 -1로 초기화
			else if (startOffsetInBlock == 0) {
				if (isDirect) inode->dirBlockPtr[i] = -1;
				else indirectBlockNumList[i - NUM_OF_DIRECT_BLOCK_PTR] = -1;
				ResetBlockBitmap(blockNum);
				UpdateNumBlockFSI(0);
				//IndirectIndex블록 초기화
				if (i == NUM_OF_DIRECT_BLOCK_PTR) {
					DevWriteBlock(inode->indirBlockPtr, block);
					ResetBlockBitmap(inode->indirBlockPtr);
					UpdateNumBlockFSI(0);
					inode->indirBlockPtr = -1;
				}
			}
			DevWriteBlock(blockNum, block);
			free(block);
		}

	}
	inode->size = offset;
	PutInode(inodeNum,inode);
	if (indirectBlockNumList != NULL) free(indirectBlockNumList);
	free(inode);
}
int ResetDir(int inodeNum, FileType type) {
	int result = -1;
	int itemCount = 0;
	int physicalCount = 0;
	if (type == FILE_TYPE_DIR) {
		DirEntryInfo* itemDirEntryList = GetDirEntryInfoList(inodeNum, &itemCount, NULL, &physicalCount);
		free(itemDirEntryList);
		if (itemCount == 2) {
			Inode* inode = (Inode*)malloc(sizeof(Inode));
			GetInode(inodeNum, inode);
			int blockNum = inode->dirBlockPtr[0];
			free(inode);
			unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
			memset(block, 0, BLOCK_SIZE);
			DevWriteBlock(blockNum, block);
			free(block);
			ResetBlockBitmap(blockNum);
			UpdateNumBlockFSI(0);
			char* temp = (char*)malloc(sizeof(Inode));
			memset(temp, 0, sizeof(Inode));
			PutInode(inodeNum, (Inode*)temp);
			free(temp);
			ResetInodeBitmap(inodeNum);
			UpdateNumInodeFSI(inodeNum);
			result = 0;
		}
		else {
			printf("[ResetDir] 안에 뭐 들어있어!!!!!!!!!!!!!!itemCount=%d\n", itemCount);
		}
	}
	return result;
}
int ResetFile(int inodeNum, FileType type) {
	int result = -1;
	if (type == FILE_TYPE_FILE) {
		Inode* inode = (Inode*)malloc(sizeof(Inode));
		GetInode(inodeNum, inode);
		int physicalBlocksCount = NUM_OF_DIRECT_BLOCK_PTR;
		int* physicalBlockNums = (int*)malloc(sizeof(int)*NUM_OF_DIRECT_BLOCK_PTR + BLOCK_SIZE);
		memcpy(physicalBlockNums, inode->dirBlockPtr, sizeof(int)*NUM_OF_DIRECT_BLOCK_PTR);
		if (inode->indirBlockPtr != -1) {
			physicalBlocksCount += BLOCK_SIZE / sizeof(int);
			DevReadBlock(inode->indirBlockPtr, (char*)(physicalBlockNums + NUM_OF_DIRECT_BLOCK_PTR));
		}
		for (int i = 0; i < physicalBlocksCount; i++) {
			if (physicalBlockNums[i] == -1) physicalBlockNums[i] = inode->indirBlockPtr;
			if (physicalBlockNums[i] == -1)break;
			unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
			memset(block, 0, BLOCK_SIZE);
			DevWriteBlock(physicalBlockNums[i], block);
			free(block);
			ResetBlockBitmap(physicalBlockNums[i]);
			UpdateNumBlockFSI(0);
		}
		free(physicalBlockNums);
		memset((char*)inode, 0, sizeof(Inode));
		PutInode(inodeNum, inode);
		free(inode);
		ResetInodeBitmap(inodeNum);
		UpdateNumInodeFSI(inodeNum);
		result = 0;
	}
	return result;
}
void RemoveLogicalBlock(int itemInodeNum, int removeLogicalBlockNum) {
	Inode* inode = (Inode*)malloc(sizeof(Inode));
	GetInode(itemInodeNum, inode);
	if (removeLogicalBlockNum < NUM_OF_DIRECT_BLOCK_PTR) {
		unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
		memset(block,0,BLOCK_SIZE);
		DevWriteBlock(inode->dirBlockPtr[removeLogicalBlockNum], block);
		free(block);
		ResetBlockBitmap(inode->dirBlockPtr[removeLogicalBlockNum]);
		UpdateNumBlockFSI(0);
		inode->dirBlockPtr[removeLogicalBlockNum] = -1;
		
	}
	
	else {
		int* indirectBlockNums = (int*)malloc(BLOCK_SIZE);
		DevReadBlock(inode->indirBlockPtr,(char*)indirectBlockNums);
		unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
		memset(block, 0, BLOCK_SIZE);
		DevWriteBlock(indirectBlockNums[removeLogicalBlockNum - NUM_OF_DIRECT_BLOCK_PTR], block);
		free(block);
		ResetBlockBitmap(indirectBlockNums[removeLogicalBlockNum - NUM_OF_DIRECT_BLOCK_PTR]);
		UpdateNumBlockFSI(0);
		indirectBlockNums[removeLogicalBlockNum - NUM_OF_DIRECT_BLOCK_PTR] = -1;
		DevWriteBlock(inode->indirBlockPtr, (char*)indirectBlockNums);
		
		free(indirectBlockNums);
		
	}
	if (removeLogicalBlockNum == NUM_OF_DIRECT_BLOCK_PTR) {
		unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
		memset(block, 0, BLOCK_SIZE);
		DevWriteBlock(inode->indirBlockPtr, block);
		free(block);
		ResetBlockBitmap(inode->indirBlockPtr);
		UpdateNumBlockFSI(0);
		inode->indirBlockPtr = -1;
	}
	PutInode(itemInodeNum, inode);
	free(inode);
}
int RemoveItem(char * itemPath, FileType fType, FileDescTable* fdTable)
{
	int result = -1;
	int isOpen = 0;
	int parentInodeNum;
	if ((parentInodeNum = GetDirInode(itemPath, -1)) > -1) {

		char itemName[MAX_NAME_LEN] = "";
		strcpy(itemName, GetItemName(itemPath));
		

		//상위 디렉토리의 엔트리리스트를 가져오자
		int* physicalBlockNums = (int *)malloc(sizeof(int)*NUM_OF_DIRECT_BLOCK_PTR+BLOCK_SIZE);
		int physicalCount = 0;
		int listCount=0;
		DirEntryInfo* dirEntryList = GetDirEntryInfoList(parentInodeNum, &listCount, physicalBlockNums, &physicalCount);
		

		//삭제할 항목을 검사하자
		int itemInodeNum; //삭제할 아이템의 InodeNo.
		int itemSeq = 0; //삭제할 아이템의 인덱스
		for (itemSeq = 0; itemSeq < listCount; itemSeq++) {
			if (strcmp(dirEntryList[itemSeq].name, itemName) == 0
				&& fType == dirEntryList[itemSeq].type) {
				itemInodeNum = dirEntryList[itemSeq].inodeNum;
				break;
			}
		}
		if (fType == FILE_TYPE_FILE) {
			for (int fdIndex = 0; fdIndex < MAX_FD_ENTRY_LEN; fdIndex++) {
				//파일을 사용중인지 확인하자!
				if (fdTable->file[fdIndex].inodeNum == itemInodeNum && (fdTable->file[fdIndex].bUsed) == 1) {
					isOpen = 1;
					printf("파일이 열려있자나~~~~!!!!!!!!!!!!!!!!!!!!!\n");
					break;
				}
			}
		}

		//항목을 상위디렉토리 엔트리에서 제거하자.
		if (isOpen==0 && itemSeq < listCount) {
			
			if(ResetDir(itemInodeNum, fType)==0 || ResetFile(itemInodeNum, fType)==0){
				
				//삭제할 항목이 있는 로지컬 블록넘버
				int logicalBlockNum = itemSeq / NUM_OF_DIRENT_PER_BLOCK;
				int seqInBlock = itemSeq % NUM_OF_DIRENT_PER_BLOCK;
				int lastLogicalBlockNum = physicalCount -1;
				int lastSeqInBlock = (listCount-1) % NUM_OF_DIRENT_PER_BLOCK;
				DirEntry* de = (DirEntry*)malloc(BLOCK_SIZE);
				DirEntry* lastDe = (DirEntry*)malloc(BLOCK_SIZE);
				DevReadBlock(physicalBlockNums[logicalBlockNum], (char*)de);
				DevReadBlock(physicalBlockNums[lastLogicalBlockNum], (char*)lastDe);

				memcpy(de+seqInBlock, lastDe+lastSeqInBlock, sizeof(DirEntry));
				//de[seqInBlock] = lastDe[lastSeqInBlock];
				lastDe[lastSeqInBlock].inodeNum = -1;
				strcpy(lastDe[lastSeqInBlock].name, "");
				DevWriteBlock(physicalBlockNums[logicalBlockNum], (char*) de);
				DevWriteBlock(physicalBlockNums[lastLogicalBlockNum],  (char*)lastDe);
				//엔트리 삭제로 인해 상위디렉토리의 마지막 블록 내에 엔트리가 비는 경우
				if (lastSeqInBlock == 0) RemoveLogicalBlock(parentInodeNum, lastLogicalBlockNum);
				free(de);
				free(lastDe);
				result = 0;
			}
		}
		free(physicalBlockNums);
		free(dirEntryList);

	}
	

	return result;
}
DirEntryInfo* GetDirEntryInfoList(int inodeNum, int* count, int* physicalBlockNums, int* physicalCount) {
	Inode* inode = (Inode*)malloc(sizeof(Inode));
	DirEntryInfo* dirEntryInfo = NULL;
	GetInode(inodeNum, inode);
	physicalCount[0] = 0;
	int flag = 0;
	int result = 0;
	for (int i = 0; i < NUM_OF_DIRECT_BLOCK_PTR; i++) {
		if (inode->dirBlockPtr[i] == -1) break;
		
		physicalBlockNums = (int*)realloc(physicalBlockNums, sizeof(int)*(++physicalCount[0]));
		physicalBlockNums[i] = inode->dirBlockPtr[i];
		DirEntry* de = (DirEntry*)malloc(BLOCK_SIZE);
		DevReadBlock(inode->dirBlockPtr[i],  (char*)de);
		for (int j = 0; j < NUM_OF_DIRENT_PER_BLOCK; j++) {
			int k = i * NUM_OF_DIRENT_PER_BLOCK + j;
			if (de[j].inodeNum == -1) {
				flag = 1;
				break;
			}
			dirEntryInfo = (DirEntryInfo*)realloc(dirEntryInfo, sizeof(DirEntryInfo)*(++result));
			strcpy(dirEntryInfo[k].name, de[j].name);
			dirEntryInfo[k].inodeNum = de[j].inodeNum;
			dirEntryInfo[k].type = GetItemType(de[j].inodeNum);
		}

		free(de);
		if (flag) break;
	}
	if (inode->indirBlockPtr > -1) {

		int* indirectBlocks = (int*)malloc(BLOCK_SIZE);
		DevReadBlock(inode->indirBlockPtr,  (char*)indirectBlocks);
		for (int i = 0; i < BLOCK_SIZE/sizeof(int); i++) {
			if (indirectBlocks[i] == -1) break;
			physicalBlockNums = (int*)realloc(physicalBlockNums, sizeof(int)*(++physicalCount[0]));
			physicalBlockNums[i+ NUM_OF_DIRECT_BLOCK_PTR] = indirectBlocks[i];
			DirEntry* de = (DirEntry*)malloc(BLOCK_SIZE);
			DevReadBlock(indirectBlocks[i],  (char*)de);

			for (int j = 0; j < NUM_OF_DIRENT_PER_BLOCK; j++) {
				int k = (i + NUM_OF_DIRECT_BLOCK_PTR) * NUM_OF_DIRENT_PER_BLOCK + j;
				if (de[j].inodeNum == -1) {
					flag = 1;
					break;
				}
				dirEntryInfo = (DirEntryInfo*)realloc(dirEntryInfo, sizeof(DirEntryInfo)*(++result));
				strcpy(dirEntryInfo[k].name, de[j].name);
				dirEntryInfo[k].inodeNum = de[j].inodeNum;
				dirEntryInfo[k].type = GetItemType(de[j].inodeNum);
			}

			free(de);

		}
	}
	free(inode);
	count[0] = result;
	return dirEntryInfo;
}
int	RWFile(int fileDesc, char* pBuffer, int length, int isWrite)
{
	
	int rwByte = 0;
	//파일의 fd 가져오기
	FileDesc fileDescItem = pFileDescTable->file[fileDesc];
	//파일의 오프셋 가져오기
	int offset = fileDescItem.fileOffset;

	//이 파일이 열린 파일이 아니라면 실패
	if (fileDescItem.bUsed == 0) return -1;

	//Open후 바로 write하는 경우 또는 file사이즈가 offset보다 클 경우
	//Inode에 필요없는 부분 날린다
	if (isWrite == 1) LoseFileBlock(offset, fileDescItem.inodeNum);
	//파일의 Inode 가져오기
	Inode* inode = (Inode*)malloc(sizeof(Inode));
	GetInode(fileDescItem.inodeNum, inode);
	
	//작업 시작할 로지컬 블록 순서
	int startLogicalBlockNum = offset / BLOCK_SIZE;
	//작업 마지막 로지컬 블록 순서
	int endLogicalBlockNum = (offset + length) / BLOCK_SIZE;
	//작업 시작할 로지컬 블록의 시작 바이트
	int startByteInStartLogicalBlockNum = offset % BLOCK_SIZE;
	//작업 마지막 로지컬 블록의 마지막 바이트
	int endByteInEndLogicalBlockNum = (offset + length) % BLOCK_SIZE;

	int* indirectIndexList = NULL;
	if (inode->indirBlockPtr > -1) {
		indirectIndexList = (int*)malloc(BLOCK_SIZE);
		DevReadBlock(inode->indirBlockPtr, (char*)indirectIndexList);
	}

	for (int i = startLogicalBlockNum; i <= endLogicalBlockNum; i++) {
		int blockNum = -1;
		int blockStartByte = 0;
		int blockCpySize = BLOCK_SIZE;
		//다이렉트블록인 경우 
		if (i < NUM_OF_DIRECT_BLOCK_PTR)
			blockNum = inode->dirBlockPtr[i];
		//인다이렉트블록으로 진입하는 경우 , 인다이렉트 블록이 할당되지 않았다면
		else if (i == NUM_OF_DIRECT_BLOCK_PTR && inode->indirBlockPtr == -1) {
			if (isWrite == 1) { //쓰기인 경우 인다이텍트 할당
				inode->indirBlockPtr = CreateIndirectBlock();
				indirectIndexList = (int*)malloc(BLOCK_SIZE);
				DevReadBlock(inode->indirBlockPtr, (char*)indirectIndexList);
			}//읽기인 경우는 중지
			else break;
			blockNum = indirectIndexList[i - NUM_OF_DIRECT_BLOCK_PTR];
		}
		//일반적인 인다이렉트 블록인경우
		else
			blockNum = indirectIndexList[i - NUM_OF_DIRECT_BLOCK_PTR];


		// 쓰기일 경우 블록번호가 inode에 할당된게 없으면 할당해 준다. 
		if (isWrite == 1 && blockNum == -1) {
			blockNum = GetFreeBlockNum();
			if (i < NUM_OF_DIRECT_BLOCK_PTR) inode->dirBlockPtr[i] = blockNum;
			else
				indirectIndexList[i - NUM_OF_DIRECT_BLOCK_PTR] = blockNum;
			//할당이 됐다면 비트맵, FSI  업데이트
			if (blockNum >= 0) {
				SetBlockBitmap(blockNum);
				UpdateNumBlockFSI(1);
			}
		}

		//블록할당이 안된 경우
		if (blockNum == -1) break;

		//디스크에서 블록 읽어오기
		unsigned char* block = (unsigned char*)malloc(BLOCK_SIZE);
		DevReadBlock(blockNum, block);
		if (i == startLogicalBlockNum) {
			blockStartByte = startByteInStartLogicalBlockNum;
			blockCpySize -= blockStartByte;
		}
		if (i == endLogicalBlockNum)
			blockCpySize = endByteInEndLogicalBlockNum;
		if (isWrite == 1)
			memcpy(block + blockStartByte, pBuffer + rwByte, blockCpySize);
		else
			memcpy(pBuffer + rwByte, block + blockStartByte, blockCpySize);
		
		rwByte += blockCpySize;
		if (isWrite == 1) DevWriteBlock(blockNum, block);
		free(block);

		// 마지막 작업 블록일 경우
		if (rwByte == length) break;

	}
	fileDescItem.fileOffset += rwByte;

	if (isWrite == 1) {
		if (indirectIndexList != NULL) DevWriteBlock(inode->indirBlockPtr, (char*)indirectIndexList);
		inode->size = fileDescItem.fileOffset;
		PutInode(fileDescItem.inodeNum, inode);
	}
	free(inode);
	pFileDescTable->file[fileDesc] = fileDescItem;

	if (indirectIndexList != NULL) free(indirectIndexList);
	return rwByte;
}


int GetItemType(int inodeNum) {
	Inode* inode = (Inode*)malloc(sizeof(Inode));
	GetInode(inodeNum, inode);
	return inode->type;
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
	fsi->blockBitmapBlock = BLOCK_BITMAP_BLOCK_NUM;
	fsi->inodeBitmapBlock = INODE_BITMAP_BLOCK_NUM;
	fsi->inodeListBlock = INODELIST_BLOCK_FIRST;
	fsi->dataRegionBlock = INODELIST_BLOCK_FIRST + INODELIST_BLOCKS;

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
char* GetItemName(char* path) {
	char* tempPath = (char*)malloc(strlen(path)+5); //path 인자의 임시
	strcpy(tempPath, "");
	if (isParentRoot(path))
		strcpy(tempPath, "ROOT");
	strcat(tempPath, path);

	// 접근할 파일/디렉 이름 만들기
	char* lastName = (char*)malloc(MAX_NAME_LEN);
	char* temp = strtok(tempPath, "/");
	while ((temp = strtok(NULL, "/")) != NULL) {
		strcpy(lastName, temp);
	}
	return lastName;
}



void PrintInode(int inodeNum) {
	Inode* inode = (Inode*)malloc(sizeof(Inode));
	GetInode(inodeNum, inode);

	printf("==========================\n");
	printf("inode[%d].type = %d\n", inodeNum, inode->type);
	printf("inode[%d].size = %d\n", inodeNum, inode->size);
	printf("inode[%d].dirBlockPtr[0] = %d\n", inodeNum, inode->dirBlockPtr[0]);
	printf("inode[%d].dirBlockPtr[1] = %d\n", inodeNum, inode->dirBlockPtr[1]);
	printf("inode[%d].indirBlockPtr = %d\n", inodeNum, inode->indirBlockPtr);
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
void printFileSys()
{
	pFileSysInfo = (struct _FileSysInfo*)malloc(BLOCK_SIZE);

	DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
	printf("--------------------fileSys--------------------\n");
	printf("blocks : %d\n", pFileSysInfo->blocks);
	printf("rootInodeNum : %d\n", pFileSysInfo->rootInodeNum);
	printf("numAllocBlocks : %d\n", pFileSysInfo->numAllocBlocks);
	printf("numFreeBlocks : %d\n", pFileSysInfo->numFreeBlocks);
	printf("numAllocInodes : %d\n", pFileSysInfo->numAllocInodes);
	printf("blockBitmapBlock : %d\n", pFileSysInfo->blockBitmapBlock);
	printf("inodeBitmapBlock : %d\n", pFileSysInfo->inodeBitmapBlock);
	printf("inodeListBlock : %d\n", pFileSysInfo->inodeListBlock);
	printf("dataReionBlock : %d\n", pFileSysInfo->dataRegionBlock);
	printf("--------------------------------------------\n\n");

}
void PrintEntryInfoList(int size, DirEntryInfo* dInfoEntry) {
	for (int i = 0; i < size; i++) {
		printf("%d  inodeNum = %d, name = %s, type = %d\n", i, dInfoEntry[i].inodeNum, dInfoEntry[i].name, dInfoEntry[i].type);
	}
}