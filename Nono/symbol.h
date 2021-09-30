#pragma once
#include "Public.h"
#include <ImageHlp.h>



#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib, "ImageHlp.lib")

#define  SystemModuleInformation 11
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)

ULONG					ListCount;


BOOLEAN EnumSymbols(char * ModuleName, EnumSymbolType	Type, PVOID  NeedList);


typedef struct _RTL_PROCESS_MODULE_INFORMATION {
	HANDLE Section;                 // Not filled in
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR  FullPathName[256];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES {
	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;



typedef NTSTATUS   (*ZwQuerySystemInformationType)(
	__in       ULONG SystemInformationClass,
	__inout    PVOID SystemInformation,
	__in       ULONG SystemInformationLength,
	__out_opt  PULONG ReturnLength
);

ZwQuerySystemInformationType	ZwQuerySystemInformation;