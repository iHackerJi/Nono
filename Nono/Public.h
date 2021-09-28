#pragma once
#include <stdio.h>
#include <windows.h>
#include <shlwapi.h>
#include <subauth.h>
#pragma comment(lib, "Shlwapi.lib")


HANDLE		hProcess;
char		CurrentDirName[MAX_PATH];

typedef enum _EnumSymbolType
{
	Symbol_Function,
	Symbol_Type
}EnumSymbolType;

typedef	struct _SymbolInfo
{
	EnumSymbolType	Type;
	ULONG64			info;
	char			Name[0x50];
}SymbolInfo, *PSymbolInfo;

typedef struct  _PakeSymbol
{
	ULONG64				SymbolNumber;
	char				ModuleName[0x20];
	SymbolInfo			SymbolInfoList[1];
}PakeSymbol, *PPakeSymbol;
