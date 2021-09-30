#pragma once
#include "GlobalSymbol.h"



#define DEVICE_NAME L"\\Device\\Nono"		
#define SYMBOL_NAME L"\\??\\Nono"			

#define NONO_TAG 'NONO'


NTSTATUS	DispatchCommon(PDEVICE_OBJECT pObject, PIRP pIrp);
NTSTATUS	DispatchIoctrl(PDEVICE_OBJECT pObject, PIRP pIrp);
NTSTATUS	InitIoComm(PDRIVER_OBJECT pDriverObj);
void		CommUnload();
void		RealDriverEntry();




