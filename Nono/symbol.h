#pragma once
#include "Public.h"

#include <ImageHlp.h>
#include <subauth.h>
#pragma comment(lib, "psapi.lib")

#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib, "ImageHlp.lib")




BOOLEAN		EnumSymbols(char * FilePath);

typedef	struct _SymbolInfo
{
	ULONG			Offset;
	UNICODE_STRING	FunctionName;
}SymbolInfo,*PSymbolInfo;

typedef struct  _PakeSymbol
{
	ULONG64				SymbolNumber;
	UNICODE_STRING		ModuleName;
	ULONG				SymbolInfoListOffset;

	PSymbolInfo	SymbolInfoList[1];
}PakeSymbol,*PPakeSymbol;


