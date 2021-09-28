#pragma once
#include "Public.h"

HANDLE	hFile;

typedef	struct _SymbolInfo
{
	ULONG			Offset;
	UNICODE_STRING	FunctionName;
}SymbolInfo, *PSymbolInfo;

typedef struct  _PakeSymbol
{
	ULONG64				SymbolNumber;
	UNICODE_STRING		ModuleName;
	ULONG				SymbolInfoListOffset;

	SymbolInfo	SymbolInfoList[1];
}PakeSymbol, *PPakeSymbol;
