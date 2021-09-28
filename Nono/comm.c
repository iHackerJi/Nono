#include "comm.h"
#define DEVICE_NAME "\\\\.\\Nono"



#define Comm_GetInfoSize CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define Comm_GetInfoBuffer CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)


BOOLEAN	InitDriverNeedInfo() {

	hFile = CreateFileA(DEVICE_NAME, GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("%s GetCurrentDirectoryA Error %d", __FUNCTION__, GetLastError());
		return	FALSE;
	}
	return TRUE;
}

BOOLEAN	GetDriverInfo()
{
	ULONG	InfoSize = 0;
	DWORD	OutLeng = 0;
	PVOID	InfoBuffer = NULL;

	if (!DeviceIoControl(hFile, Comm_GetInfoSize, NULL, 0, &InfoSize, sizeof(InfoSize), &OutLeng, NULL))
	{
		printf("%s DeviceIoControl Error %d", __FUNCTION__, GetLastError());
		return	FALSE;
	}

	InfoBuffer = malloc(InfoSize);
	if (NULL == InfoBuffer)
	{
		printf("%s malloc Error %d", __FUNCTION__, GetLastError());
		return	FALSE;
	}
	if (!DeviceIoControl(hFile, Comm_GetInfoSize, NULL, 0, &InfoBuffer, InfoSize, &OutLeng, NULL))
	{
		printf("%s DeviceIoControl Error %d", __FUNCTION__, GetLastError());
		return	FALSE;
	}




}