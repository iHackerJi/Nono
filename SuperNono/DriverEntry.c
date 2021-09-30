#include <ntifs.h>
#include "comm.h"


void	UnloadDriver(PDRIVER_OBJECT	pDriverObj) 
{
	CommUnload();

	return;
}



NTSTATUS	DriverEntry(PDRIVER_OBJECT	pDriverObj,PUNICODE_STRING	pReg) 
{
	NTSTATUS	Status = STATUS_SUCCESS;
	pDriverObj->DriverUnload = UnloadDriver;


	Status = InitIoComm(pDriverObj);
	if (!NT_SUCCESS(Status))	return STATUS_UNSUCCESSFUL;

	return	Status;
}

void	RealDriverEntry() {
	//表示我们的函数与偏移已经初始化完毕了，可以做事情了




}