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


	hProcess = GetCurrentProcess();

	//提升调试权限
	if (!EnableDebugPriv()) 
	{
		printf("%s EnableDebugPriv Error %d", __FUNCTION__, GetLastError());
		return	FALSE;
	}



	//设置调试选项
	SymSetOptions(SYMOPT_CASE_INSENSITIVE | SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);

	//清除调试句柄
	SymCleanup(hProcess);

	if (-1 == sprintf(SymbolPath, "SRV*%s*http://msdl.microsoft.com/download/symbols", SymbolDownloadPath))
	{
		printf("%s sprintf Error %d", __FUNCTION__, GetLastError());
		return	FALSE;
	}

	if (!SymInitialize(hProcess, SymbolPath, FALSE))
	{
		printf("%s SymInitialize Error %d", __FUNCTION__, GetLastError());
		return	FALSE;
	}


	//创建这个文件是为了防止提示一个对话框
	sprintf(SymsrvYesName, "%s\\symsrv.yes", CurrentDirName);
	if (!PathFileExistsA(SymsrvYesName))
	{
		if (INVALID_HANDLE_VALUE == CreateFileA(SymsrvYesName, FILE_ALL_ACCESS, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))
		{
			printf("%s CreateFileA Error %d", __FUNCTION__, GetLastError());
			return	FALSE;
		}
	}
	


	return	 TRUE;
}

BOOL EnumSymRoutine(
	PSYMBOL_INFO pSymInfo,
	ULONG SymbolSize,
	PVOID UserContext
) {

	return	TRUE;
}


BOOLEAN EnumSymbols(char * ModuleName) {

	char SymFileName[MAX_PATH] =  { 0 };
	char ModuleNamePath[MAX_PATH];

	PLOADED_IMAGE	pImage = NULL;
	BOOLEAN			Ret = FALSE;

	//取出系统模块地址
	if (!GetSystemDirectoryA(ModuleNamePath, MAX_PATH))
	{
		printf("%s GetSystemDirectoryA Error %d", __FUNCTION__, GetLastError());
		return FALSE;
	}

	if (-1 == sprintf(ModuleNamePath, "%s\\%s", ModuleNamePath, ModuleName))
	{
		printf("%s sprintf Error %d", __FUNCTION__, GetLastError());
		return FALSE;
	}
	
	//初始化符号路径
	if (!InitSymbols(CurrentDirName)) 
	{
		return FALSE;
	}
	
	if (!PathFileExistsA(ModuleNamePath))
	{
		printf("%s PathFileExistsA Error %d", __FUNCTION__, GetLastError());
		return FALSE;
	}

	//下载符号文件
	if (!SymGetSymbolFile(hProcess, NULL, ModuleNamePath, sfPdb, SymFileName, MAX_PATH, SymFileName, MAX_PATH))
	{
		printf("%s SymGetSymbolFile Error %d", __FUNCTION__, GetLastError());
		return FALSE;
	}

	//加载映像
	pImage = ImageLoad(ModuleNamePath, NULL);
	if (pImage == NULL)
	{
		printf("%s ImageLoad Error %d", __FUNCTION__, GetLastError());
		goto _Exit;
	}

	//加载符号并解析
	if (!SymLoadModule64(hProcess, pImage->hFile, pImage->ModuleName, NULL, (DWORD64)pImage->MappedAddress, pImage->SizeOfImage))
	{
		printf("%s SymLoadModule64 Error %d", __FUNCTION__, GetLastError());
		goto _Exit;
	}
	
	if (!SymEnumSymbols(hProcess, (ULONG64)pImage->MappedAddress, NULL, EnumSymRoutine, NULL))
	{
		printf("%s SymEnumSymbols Error %d", __FUNCTION__, GetLastError());
		goto _Exit;
	}
	Ret = TRUE;


_Exit:
	if (pImage)	ImageUnload(pImage);
	return Ret;
}