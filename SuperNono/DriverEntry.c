#include <ntifs.h>



void	UnloadDriver(PDRIVER_OBJECT	pDriverObj) 
{

	return;
}


NTSTATUS	DriverEntry(PDRIVER_OBJECT	pDriverObj,PUNICODE_STRING	pReg) {

	pDriverObj->DriverUnload = UnloadDriver;





	return	STATUS_SUCCESS;
}