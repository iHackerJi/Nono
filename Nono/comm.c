#include "comm.h"

BOOLEAN	InitDriverNeedInfo(char * InfoFile) {
	char InfoFilePath[MAX_PATH] = { 0 };
	HFILE	hFile = 0;
	POFSTRUCT	ReOpenBuff = {0};
	//��ȡ��ǰ·��
	if (0 == GetCurrentDirectoryA(MAX_PATH, CurrentDirName))
	{
		printf("%s GetCurrentDirectoryA Error %d", __FUNCTION__, GetLastError());
		return	FALSE;
	}

	if (-1 == sprintf(InfoFilePath, "%s//%s", CurrentDirName, InfoFile))
	{
		printf("%s sprintf Error %d", __FUNCTION__, GetLastError());
		return	FALSE;
	}
	
	if (!PathFileExistsA(InfoFilePath))
	{
		printf("%s InfoFilePath Error %d", __FUNCTION__, GetLastError());
		return FALSE;
	}

	//��ʽ���£�
	//ģ����:������.������.������.������|
	//ģ����:������.������.������.������|
	//
	hFile = OpenFile(InfoFilePath, ReOpenBuff, OF_READ);
	if (HFILE_ERROR == hFile)
	{
		printf("%s OpenFile Error %d", __FUNCTION__, GetLastError());
		return FALSE;
	}


	return TRUE;
}