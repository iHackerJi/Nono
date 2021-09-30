#include <ntifs.h>
#include "comm.h"






void	UnloadDriver(PDRIVER_OBJECT	pDriverObj) 
{
	CommUnload();

	return;
}


NTSTATUS	DriverEntry(PDRIVER_OBJECT	pDriverObj,PUNICODE_STRING	pReg) 
{

	pDriverObj->DriverUnload = UnloadDriver;
	InitIoComm(pDriverObj);

	 return	STATUS_SUCCESS;
}