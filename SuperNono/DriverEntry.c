#include <ntifs.h>
#include "comm.h"

UNICODE_STRING	DeviceName = RTL_CONSTANT_STRING(DEVICE_NAME);
UNICODE_STRING	SymbolName = RTL_CONSTANT_STRING(SYMBOL_NAME);
PDRIVER_OBJECT	g_pDriverObj = NULL;
BOOLEAN			DeviceAndSymbolLinkDelete = FALSE;



void	UnloadDriver(PDRIVER_OBJECT	pDriverObj) 
{
	if (!DeviceAndSymbolLinkDelete)
	{
		IoDeleteDevice(pDriverObj->DeviceObject);
		IoDeleteSymbolicLink(&SymbolName);
	}

	return;
}


NTSTATUS	DriverEntry(PDRIVER_OBJECT	pDriverObj,PUNICODE_STRING	pReg) 
{
	PDEVICE_OBJECT	pDeviceObj = NULL;
	NTSTATUS		status = STATUS_SUCCESS;
	g_pDriverObj = pDriverObj;

	 pDriverObj->DriverUnload = UnloadDriver;
	 

	 for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
	 	pDriverObj->MajorFunction[i] = DispatchCommon;
	 
	 pDriverObj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoctrl;
	 
	 
	 status = IoCreateDevice(pDriverObj, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pDeviceObj);
	 if (!NT_SUCCESS(status)) {
	 	KdPrint(("IoCreateDevice%08X\n", status));
	 	return(status);
	 }
	 
	 pDeviceObj->Flags |= DO_DIRECT_IO;
	 
	 status = IoCreateSymbolicLink(&SymbolName, &DeviceName);
	 if (!NT_SUCCESS(status)) {
	 	KdPrint(("IoCreateSymbolicLink\n"));
	 	return(status);
	 }


	 
	 
	 return	STATUS_SUCCESS;
}