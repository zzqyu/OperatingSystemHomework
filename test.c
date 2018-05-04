#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "fs.h"

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
	Mount(MT_TYPE_FORMAT);
	
	//PrintDirEntryList(19);
	
	MakeDir("/test");
	MakeDir("/test2");
	printf("fd = %d\n", OpenFile("/a.c", OPEN_FLAG_CREATE));
	MakeDir("/test3");
	MakeDir("/test4");
	MakeDir("/test5");
	MakeDir("/test6");
	MakeDir("/test7");
	MakeDir("/test7/test8");
	printf("fd = %d\n", OpenFile("/test7/b.c", OPEN_FLAG_CREATE));

	
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
}