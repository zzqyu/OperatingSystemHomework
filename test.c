#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>

#include "fs.h"
#include "own_func.h"

//#define MAX_NAME_LEN (20)

/*
void test(char* path) {
	int flag = 0;
	int pathLength = strlen(path);
	char* tempPath = (char*)malloc(pathLength);
	char parentDirName[MAX_NAME_LEN], thisDirName[MAX_NAME_LEN], lastDirName[MAX_NAME_LEN];
	char* childPath = (char*)malloc(pathLength);
	strcpy(tempPath, path);
	if (path[0] == '/') {
		strcpy(tempPath, "ROOT");
		strcat(tempPath, path);
	}
	strcpy(parentDirName, strtok(tempPath, "/"));
	strcpy(thisDirName, strtok(NULL, "/"));
	strcpy(childPath, thisDirName);
	char* temp = strtok(NULL, "\0");
	if (temp != NULL) {
		strcat(childPath, "/");
		strcat(childPath, temp);
		flag = 1;
	}
	strtok(path, "/");
	while ((temp = strtok(NULL, "/")) != NULL) {
		strcpy(lastDirName, temp);
	}
	
	printf("=======================\n");
	printf("parentDirName = '%s'\n", parentDirName);
	printf("thisDirName = '%s'\n", thisDirName);
	printf("childPath = '%s'\n", childPath);
	printf("lastDirName = '%s'\n", lastDirName);
	if(flag==1)test(childPath);
}*/
void main() {
	//char path[128] = "/a.c";///sdasda/asdasd/dfsdf";
	//test(path);
	int fd1, fd2;
	
	//PrintDirEntryList(19);
	Mount(MT_TYPE_FORMAT);
	MakeDir("/test");
	MakeDir("/test2");
	printf("fd1 = %d\n", fd1=OpenFile("/a.c", OPEN_FLAG_CREATE));
	MakeDir("/test3");
	MakeDir("/test4");
	MakeDir("/test5");
	MakeDir("/test6");
	MakeDir("/test7");
	MakeDir("/test8");
	MakeDir("/test9");
	MakeDir("/test10");
	MakeDir("/test11");
	MakeDir("/test12");
	MakeDir("/test13");
	printf("fd1 = %d\n", fd2=OpenFile("/test7/b.c", OPEN_FLAG_CREATE));

	
	PrintInode(0);
	PrintInode(1);
	PrintInode(2);
	PrintInode(3);
	PrintInode(4);
	PrintInode(5);
	PrintInode(6);
	PrintInode(7);
	PrintInode(8);
	PrintInode(9);
	PrintDirEntryList(19);
	PrintDirEntryList(20);
	PrintDirEntryList(21);
	PrintDirEntryList(22);
	PrintDirEntryList(23);
	PrintDirEntryList(24);
	PrintDirEntryList(25);
	PrintDirEntryList(26);
	PrintIndirectIndexList(27);
	PrintDirEntryList(28);
	PrintDirEntryList(29);
	PrintDirEntryList(30);
	setlocale(LC_ALL, "");
	printf("=========================\n");
	printf("%ls", L"한글");
	printf("\n");
	char result[] = "wadjaksasdkjasslsajasasladlasclkxjxiojdslckslx";//"가나다라마바사아자차카타파하하가나다라마바사아자차카타파하";
	char result1[] = "sdkjsd!!!!!!!!!!!!!!!";//"시바바시바바시바바시바바";
	int leng = WriteFile(fd1, result, strlen(result));
	printf("WriteFile leng = %d, WriteString = %s \n", leng, result);
	leng = WriteFile(fd1, result1, strlen(result1));
	printf("WriteFile leng = %d, WriteString = %s \n", leng, result1);
	printf("WriteStringBit = %s\n", BlockToBinary(64, result));
	CloseFile(fd1);
	printf("Openfile fd1 = %d\n", fd1 = OpenFile("/a.c", OPEN_FLAG_READWRITE));
	char* result2 = (char*)malloc(69);
	printf("리드전\n");
	leng = ReadFile(fd1, result2, strlen(result)+ strlen(result1));
	printf("Read result = '" );
	//result2[46] = '!';
	printf("%s", result2);
	printf("' ReadFile leng = %d\n", leng);
	//printf(" ReadStringBit = %s\n", BlockToBinary(64, result1));

	CloseFile(fd1);
	
	RemoveDir("/test3");
	RemoveFile("/a.c");
	

	PrintIndirectIndexList(27);
	PrintInode(0);
	

	printf("\n\n");
	int itemCount;
	DirEntryInfo* pDirEntry = (DirEntryInfo*)malloc(sizeof(DirEntryInfo)*15);
	PrintEntryInfoList(itemCount = EnumerateDirStatus("/", pDirEntry, 15), pDirEntry);

	RemoveDir("/test5");

	PrintIndirectIndexList(27);
	PrintInode(0);


	pDirEntry = (DirEntryInfo*)malloc(sizeof(DirEntryInfo) * 15);
	PrintEntryInfoList(itemCount = EnumerateDirStatus("/", pDirEntry, 15), pDirEntry);RemoveDir("/test5");

	RemoveDir("/test10");
	RemoveDir("/test13");
	RemoveDir("/test12");
	RemoveDir("/test6");
	RemoveDir("/test7");
	PrintIndirectIndexList(27);
	PrintInode(0);


	pDirEntry = (DirEntryInfo*)malloc(sizeof(DirEntryInfo) * 15);
	PrintEntryInfoList(itemCount = EnumerateDirStatus("/", pDirEntry, 15), pDirEntry);

	return;
}