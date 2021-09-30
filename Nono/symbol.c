#include "symbol.h"

BOOLEAN EnableDebugPriv(void)
{
	HANDLE hToken;

	LUID sedebugnameValue;

	TOKEN_PRIVILEGES tkp;

	if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return FALSE;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		CloseHandle(hToken);

		return FALSE;
	}
	tkp.PrivilegeCount = 1;

	tkp.Privileges[0].Luid = sedebugnameValue;

	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		CloseHandle(hToken);

		return FALSE;
	}
	return TRUE;
}



BOOLEAN	InitSymbols(char * SymbolDownloadPath)
{
	char	SymbolPath[MAX_PATH] = { 0 };
	int		Ret = 0;
	char	SymsrvYesName[MAX_PATH] = { 0 };
	char	CurrentDirName[MAX_PATH];


	if (0 == GetCurrentDirectoryA(MAX_PATH, CurrentDirName))
	{
		printf("%s GetCurrentDirectoryA Error %d \r\n", __FUNCTION__, GetLastError());
		return	FALSE;
	}


	hProcess = GetCurrentProcess();

	//提升调试权限
	if (!EnableDebugPriv()) 
	{
		printf("%s EnableDebugPriv Error %d \r\n", __FUNCTION__, GetLastError());
		return	FALSE;
	}



	//设置调试选项
	SymSetOptions(SYMOPT_CASE_INSENSITIVE | SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);

	//清除调试句柄
	SymCleanup(hProcess);

	if (-1 == sprintf(SymbolPath, "SRV*%s*http://msdl.microsoft.com/download/symbols", SymbolDownloadPath))
	{
		printf("%s sprintf Error %d \r\n", __FUNCTION__, GetLastError());
		return	FALSE;
	}

	if (!SymInitialize(hProcess, SymbolPath, FALSE))
	{
		printf("%s SymInitialize Error %d \r\n", __FUNCTION__, GetLastError());
		return	FALSE;
	}


	//创建这个文件是为了防止提示一个对话框
	sprintf(SymsrvYesName, "%s\\symsrv.yes", CurrentDirName);
	if (!PathFileExistsA(SymsrvYesName))
	{
		if (INVALID_HANDLE_VALUE == CreateFileA(SymsrvYesName, FILE_ALL_ACCESS, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))
		{
			printf("%s CreateFileA Error %d \r\n", __FUNCTION__, GetLastError());
			return	FALSE;
		}
	}
	


	return	 TRUE;
}

BOOL EnumSymFunctionRoutine(
	PSYMBOL_INFO pSymInfo,
	ULONG SymbolSize,
	PVOID UserContext
) {


	PSymbolGetFunction		Info = (PSymbolGetFunction)UserContext;

	ULONG					ListNumber = 0;
	for (ULONG i = 0; i < ListCount ; i++)
	{

		if (strcmp(pSymInfo->Name,Info[i].Name) == 0)
		{

			Info[i].ReceiveFunction = (PVOID*)pSymInfo->Address;
			printf("解析到 %s %p \r\n ", pSymInfo->Name, (PVOID)pSymInfo->Address);
		}
	}

	return	TRUE;
}

BOOL EnumSymTyoeRoutine(
	PSYMBOL_INFO pSymInfo,
	ULONG SymbolSize,
	PVOID UserContext
) 
{

	PSymbolGetTypeOffset		Info = (PSymbolGetTypeOffset)UserContext;
	BOOLEAN						IsFlags = FALSE;


	for (ULONG K = 0; K <ListCount ; K++)
	{

		if (strcmp(pSymInfo->Name, Info[K].ParentName) == 0)
		{
			IsFlags = TRUE;
		}
	}

	if (IsFlags == FALSE)	return TRUE;


	PVOID						ModuleBase = (PVOID)pSymInfo->ModBase;
	WCHAR						SonName[Symbol_NameLength] = {0};

	for (ULONG K = 0; K < ListCount; K++)
	{

		if (strcmp(pSymInfo->Name, Info[K].ParentName) == 0)
		{
			TI_FINDCHILDREN_PARAMS *	SonList = NULL;

			DWORD						SonListSize = 0;
			DWORD						SonCount = 0;
			BOOLEAN						Ret = TRUE;
			PWCHAR						TempName = NULL;
			ULONG64						Offset = 0;


			Ret = SymGetTypeInfo(hProcess, (DWORD64)ModuleBase, pSymInfo->Index, TI_GET_CHILDRENCOUNT, &SonCount);
			if (!Ret)
			{
				printf("SymGetTypeInfo Erro =%d \r\n", GetLastError());
				goto _Exit;
			}
			SonListSize = sizeof(TI_FINDCHILDREN_PARAMS) + sizeof(ULONG) * SonCount;
			SonList = malloc(SonListSize);
			ZeroMemory(SonList, SonListSize);
			SonList->Count = SonCount;	//一定要设置数量，否则拿不到

			Ret = SymGetTypeInfo(hProcess, (DWORD64)ModuleBase, pSymInfo->Index, TI_FINDCHILDREN, SonList);
			if (!Ret)
			{
				printf("SymGetTypeInfo Erro =%d \r\n", GetLastError());
				goto _Exit;
			}

			for (ULONG i = 0; i < SonCount; i++)
			{
				swprintf(SonName, Symbol_NameLength, L"%hs", Info[K].SonName);
				SymGetTypeInfo(hProcess, (DWORD64)ModuleBase, SonList->ChildId[i], TI_GET_SYMNAME, &TempName);
		
				if (wcscmp(TempName, SonName) == 0)
				{
					Ret = SymGetTypeInfo(hProcess, (DWORD64)ModuleBase, SonList->ChildId[i], TI_GET_OFFSET, &Offset);
					if (!Ret)
					{
						printf("SymGetTypeInfo Erro =%d \r\n", GetLastError());
						VirtualFree(TempName, 0, MEM_RELEASE);
						goto _Exit;
					}
					Info[K].Offset = (PULONG64)Offset;
					printf("解析到 结构 %s %s %x \r\n", Info[K].ParentName, Info[K].SonName, (UINT)Offset);
				}
				VirtualFree(TempName, 0, MEM_RELEASE);
			}



		_Exit:
			if (SonList)	free(SonList);

		}
	}
	return TRUE;
}


BOOLEAN EnumSymbols(char * ModuleName,EnumSymbolType	Type,PVOID  NeedList) 
{
	
	char SymFileName[MAX_PATH] =  { 0 };
	char ModuleNamePath[MAX_PATH] = { 0 };

	char SystemDir[MAX_PATH] = { 0 };
	char SymbolPath[MAX_PATH] = { 0 };
	char Symbol[MAX_PATH] = "NonoSymbol";

	PRTL_PROCESS_MODULES	pModule = NULL;
	BOOLEAN					Ret = FALSE;
	ULONG					RetLeng = 0;
	NTSTATUS				Status = STATUS_SUCCESS;

	PVOID					ModuleBase = NULL;
	DWORD					ModuleSize = 0;
	PLOADED_IMAGE			pImage = NULL;


	//取出系统模块地址
	if (!GetSystemDirectoryA(SystemDir, MAX_PATH))
	{
		printf("%s GetSystemDirectoryA Error %d \r\n", __FUNCTION__, GetLastError());
		return FALSE;
	}

	if (-1 == sprintf(ModuleNamePath, "%s\\%s", SystemDir, ModuleName))
	{
		printf("%s sprintf Error %d \r\n", __FUNCTION__, GetLastError());
		return FALSE;
	}

	if (-1 == sprintf(SymbolPath, "%s\\%s", SystemDir, Symbol))
	{
		printf("%s sprintf Error %d \r\n", __FUNCTION__, GetLastError());
		return FALSE;
	}

	
	//初始化符号路径
	if (!InitSymbols(SymbolPath))
	{
		return FALSE;
	}
	
	if (!PathFileExistsA(ModuleNamePath))
	{
		printf("%s PathFileExistsA Error %d \r\n", __FUNCTION__, GetLastError());
		return FALSE;
	}

	//下载符号文件
	if (!SymGetSymbolFile(hProcess, NULL, ModuleNamePath, sfPdb, SymFileName, MAX_PATH, SymFileName, MAX_PATH))
	{
		printf("%s SymGetSymbolFile Error %d \r\n", __FUNCTION__, GetLastError());
		return FALSE;
	}

	ZwQuerySystemInformation = (ZwQuerySystemInformationType)GetProcAddress(LoadLibraryA("ntdll.dll"), "ZwQuerySystemInformation");
	if (ZwQuerySystemInformation == NULL)
	{
		printf("%s GetZwQuerySystemInformation Error %d \r\n", __FUNCTION__, GetLastError());
		return FALSE;
	}

	//取出系统模块的地址
	Status = ZwQuerySystemInformation(SystemModuleInformation, pModule, 0, &RetLeng);
	if (Status != STATUS_INFO_LENGTH_MISMATCH)
	{
		printf("%s ZwQuerySystemInformation Error %d \r\n", __FUNCTION__, GetLastError());
		return FALSE;
	}

	pModule = malloc(RetLeng);
	memset(pModule, 0, RetLeng);
	Status = ZwQuerySystemInformation(SystemModuleInformation, pModule, RetLeng, &RetLeng);
	if (Status != STATUS_SUCCESS)
	{
		printf("%s ZwQuerySystemInformation Error %d \r\n", __FUNCTION__, GetLastError());
		goto _Exit;
	}

	for (ULONG i = 0; i < pModule->NumberOfModules; i++)
	{
		//循环从链表对比
		if (strstr(pModule->Modules[i].FullPathName, ModuleName))
		{
			ModuleBase = pModule->Modules[i].ImageBase;
			ModuleSize = pModule->Modules[i].ImageSize;
			break;
		}
	}

	if (ModuleBase == NULL && ModuleSize == 0)
	{
		printf("%s GetModule Error %d \r\n", __FUNCTION__, GetLastError());
		goto _Exit;
	}


	pImage = ImageLoad(ModuleNamePath, NULL);
	if (pImage == NULL)
	{
		printf("%s ImageLoad Error %d \r\n", __FUNCTION__, GetLastError());
		goto _Exit;
	}


	//加载符号并解析
	if (!SymLoadModule64(hProcess, pImage->hFile, pImage->ModuleName, NULL, (DWORD64)ModuleBase, ModuleSize))
	{
		printf("%s SymLoadModule64 Error %d \r\n", __FUNCTION__, GetLastError());
		goto _Exit;
	}
	
	//枚举符号

	switch (Type)
	{
		case Symbol_Function:
		{

			PSymbolGetFunction		Info = (PSymbolGetFunction)NeedList;
		
			ListCount = 0;

			for (int i = 0 ;  i < Symbol_InfoListMax ; i++)
			{
				if (strcmp(Info[i].Name, Symbol_MaxListFlag) == 0)
				{
					ListCount = i;
					break;
				}
			}

			if (!SymEnumSymbols(hProcess, (ULONG64)ModuleBase, NULL, EnumSymFunctionRoutine, NeedList))
			{
				printf("%s SymEnumSymbols Error %d \r\n", __FUNCTION__, GetLastError());
				goto _Exit;
			}
			break;
		}
		case Symbol_Type:
		{


			PSymbolGetTypeOffset		Info = (PSymbolGetTypeOffset)NeedList;

			ListCount = 0;

			for (int i = 0; i < Symbol_InfoListMax; i++)
			{
				if (strcmp(Info[i].ParentName, Symbol_MaxListFlag) == 0)
				{
					ListCount = i;
					break;
				}
			}


			SymEnumTypes(hProcess, (ULONG64)ModuleBase, EnumSymTyoeRoutine, NeedList);
			break;
		}
		default:	goto _Exit;
	}

	Ret = TRUE;

_Exit:
	if (pModule)	free(pModule);
	if (pImage)	ImageUnload(pImage);

	return Ret;
}