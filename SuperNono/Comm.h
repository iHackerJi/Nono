#pragma once
#include <ntifs.h>
#include "../SymbolShare.h"

#define DEVICE_NAME L"\\Device\\Nono"		
#define SYMBOL_NAME L"\\??\\Nono"			

#define NONO_TAG 'NONO'


#define		Symbol_NameLength			0x50
#define		Symbol_ModuleNameLength		0x20
#define		Symbol_InfoListMax			0x20

#define		Symbol_MaxListFlag		"_MaxList"


NTSTATUS DispatchCommon(PDEVICE_OBJECT pObject, PIRP pIrp);
NTSTATUS DispatchIoctrl(PDEVICE_OBJECT pObject, PIRP pIrp);






