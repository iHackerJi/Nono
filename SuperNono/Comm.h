#pragma once
#include <ntifs.h>

#define DEVICE_NAME L"\\Device\\Nono"		
#define SYMBOL_NAME L"\\??\\Nono"			


#define IOCTRL_BASE 0x800

#define MYIOCTRL_CODE(i) \
	CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTRL_BASE+i, METHOD_BUFFERED,FILE_ANY_ACCESS)

#define CTL_HELLO MYIOCTRL_CODE(0)
#define CTL_AcceptSymbolInfo MYIOCTRL_CODE(1)
#define CTL_BYE MYIOCTRL_CODE(2)


#define		Symbol_NameLength			0x50
#define		Symbol_ModuleNameLength		0x20
#define		Symbol_NameListNumber		0x50

#define		Symbol_MaxList		"_MaxList"

typedef enum _EnumSymbolType
{
	Symbol_Function,
	Symbol_Type
}EnumSymbolType;

typedef	struct _SymbolInfo
{
	EnumSymbolType	Type;
	ULONG64			info;
	char			Name[Symbol_NameLength];
}SymbolInfo, *PSymbolInfo;

typedef struct  _PakeSymbol
{
	ULONG64				SymbolNumber;
	char				ModuleName[Symbol_ModuleNameLength];
	SymbolInfo			SymbolInfoList[1];
}PakeSymbol, *PPakeSymbol;

typedef struct _SymbolGetName
{
	char		Name[Symbol_NameLength];
	ULONG64		Info;
}SymbolGetName,*PSymbolGetName;

typedef struct _SymbolGetInfoList
{
	char				ModuleName[Symbol_ModuleNameLength];
	SymbolGetName		NameList[Symbol_NameListNumber];
}SymbolGetInfoList, *PSymbolGetInfoList;