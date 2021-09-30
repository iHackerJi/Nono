#include "comm.h"
#define DEVICE_NAME "\\\\.\\Nono"



BOOLEAN	InitComm() 
{

	hFile = CreateFileA(DEVICE_NAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("%s GetCurrentDirectoryA Error %d \r\n", __FUNCTION__, GetLastError());
		return	FALSE;
	}
	return TRUE;
}

BOOLEAN	InitSymbolFunctionList()
{
	DWORD							OutLeng = 0;
	InfoOfSizeList					InfoOfSize = {0};
	PSymbolGetFunctionInfoList		FunInfo = NULL;
	BOOLEAN							Res = FALSE;

	if (!DeviceIoControl(hFile, CTL_GetFunListSize, NULL, 0, &InfoOfSize, sizeof(InfoOfSize), &OutLeng, NULL))
	{
		printf("%s DeviceIoControl CTL_GetFunListSize Error %d \r\n", __FUNCTION__, GetLastError());
		return	FALSE;
	}

	printf("InfoofSize.StructSize = %d \r\n", InfoOfSize.StructSize);

	FunInfo = (PSymbolGetFunctionInfoList)malloc(InfoOfSize.StructSize);

	if (!FunInfo)
	{
		printf("%s malloc  FunInfo Error  \r\n", __FUNCTION__);
		return	FALSE;
	}

	if (!DeviceIoControl(hFile, CTL_GetFunListInfo, NULL, 0, FunInfo, InfoOfSize.StructSize, &OutLeng, NULL))
	{
		printf("%s DeviceIoControl  CTL_GetFunListInfo Error %d \r\n", __FUNCTION__, GetLastError());
		return	FALSE;
	}


	printf("准备解析符号表\r\n");
	for (ULONG i = 0; i < InfoOfSize.ListCount; i++)
	{
		if (!EnumSymbols(FunInfo[i].ModuleName, Symbol_Function, FunInfo[i].InfoList))
		{
			printf("%s EnumSymbols Error %d \r\n", __FUNCTION__, GetLastError());
			goto	_Exit;
		}
		
	}

	if (!DeviceIoControl(hFile, CTL_SendFunListInfo, FunInfo, InfoOfSize.StructSize, NULL, 0, &OutLeng, NULL))
	{
		printf("%s DeviceIoControl  CTL_GetFunListInfo Error %d \r\n", __FUNCTION__, GetLastError());
		goto	_Exit;
	}

	Res = TRUE;

_Exit:
	free(FunInfo);


	return Res;
}

BOOLEAN	InitSymbolTypeList() 
{
	DWORD							OutLeng = 0;
	InfoOfSizeList					InfoOfSize = { 0 };
	PSymbolGetTypeOffsetList		TypeInfo = NULL;
	BOOLEAN							Res = FALSE;

	if (!DeviceIoControl(hFile, CTL_GetTypeListSize, NULL, 0, &InfoOfSize, sizeof(InfoOfSize), &OutLeng, NULL))
	{
		printf("%s DeviceIoControl CTL_GetFunListSize Error %d \r\n", __FUNCTION__, GetLastError());
		return	FALSE;
	}

	printf("Type InfoofSize.StructSize = %d \r\n", InfoOfSize.StructSize);

	TypeInfo = (PSymbolGetTypeOffsetList)malloc(InfoOfSize.StructSize);

	if (!TypeInfo)
	{
		printf("%s malloc  FunInfo Error  \r\n", __FUNCTION__);
		return	FALSE;
	}

	if (!DeviceIoControl(hFile, CTL_GetTypeListInfo, NULL, 0, TypeInfo, InfoOfSize.StructSize, &OutLeng, NULL))
	{
		printf("%s DeviceIoControl  CTL_GetFunListInfo Error %d \r\n", __FUNCTION__, GetLastError());
		return	FALSE;
	}


	printf("准备解析符号表\r\n");
	for (ULONG i = 0; i < InfoOfSize.ListCount; i++)
	{
		if (!EnumSymbols(TypeInfo[i].ModuleName, Symbol_Type, TypeInfo[i].InfoList))
		{
			printf("%s EnumSymbols Error %d \r\n", __FUNCTION__, GetLastError());
			goto	_Exit;
		}

	}

	if (!DeviceIoControl(hFile, CTL_SendTypeListInfo, TypeInfo, InfoOfSize.StructSize, NULL, 0, &OutLeng, NULL))
	{
		printf("%s DeviceIoControl  CTL_GetFunListInfo Error %d \r\n", __FUNCTION__, GetLastError());
		goto	_Exit;
	}

	Res = TRUE;

_Exit:
	free(TypeInfo);


	return Res;
}

void	DeleteMark()
{
	DWORD							OutLeng = 0;
	DeviceIoControl(hFile, CTL_DeleteMark, NULL, 0, NULL, 0, &OutLeng, NULL);
}

