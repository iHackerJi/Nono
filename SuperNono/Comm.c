#include "Comm.h"

int	GetFunctionCount = 0;
int	GetTypeOffsetCount = 0;



static	SymbolGetInfoList	g_GetFunctionInfoList[] =
{
	{
		"ntoskrnl.exe",
		{
			{"WMIInitialize",0},
			{"PipAddDevicesToBootDriverWorker", 0},
			{"PnpInitializeBootStartDriver", 0},
			{Symbol_MaxList, 0},
		}
	},
	{
		"win32k.sys",
		{
			{"NtUserShowCaret",0},
			{"NtUserShowScrollBar",0},
			{"NtUserSetCapture",0},
		}
	}
};


void	HelloRoutine() 
{
	ULONG	GetFunctionInfoListNumber = 0;
	ULONG	NameNumber = 0;
	GetFunctionInfoListNumber =	sizeof(g_GetFunctionInfoList) / sizeof(char) * Symbol_ModuleNameLength + sizeof(SymbolGetName) * Symbol_NameListNumber;


	for (int i =0 ; i < GetFunctionInfoListNumber; i ++)
	{
		
		for (int j =0 ; j < Symbol_NameListNumber ; j++)
		{
			if (strcmp(g_GetFunctionInfoList[i].NameList[j].Name, Symbol_MaxList) == 0)
			{
				break;
			}

		}

	}

}


NTSTATUS DispatchCommon(PDEVICE_OBJECT pObject, PIRP pIrp)
{

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DispatchIoctrl(PDEVICE_OBJECT pObject, PIRP pIrp)
{

	ULONG uIoctrlCode = 0;
	PVOID pInputBuff = NULL;
	PVOID pOutputBuff = NULL;

	ULONG uInputLength = 0;
	ULONG uOutputLength = 0;
	PIO_STACK_LOCATION pStack = NULL;

	pInputBuff = pOutputBuff = pIrp->AssociatedIrp.SystemBuffer;

	pStack = IoGetCurrentIrpStackLocation(pIrp);
	uInputLength = pStack->Parameters.DeviceIoControl.InputBufferLength;
	uOutputLength = pStack->Parameters.DeviceIoControl.OutputBufferLength;


	uIoctrlCode = pStack->Parameters.DeviceIoControl.IoControlCode;

	switch (uIoctrlCode)
	{
	case CTL_HELLO:
	{
		DbgPrint("Hello iocontrol\n");
		break;
	}
	case CTL_AcceptSymbolInfo:
	{
		DbgPrint("%ws\n", pInputBuff);
		break;
	}
	case CTL_BYE:
	{
		DbgPrint("Goodbye iocontrol\n");
		break;
	}
	default:
		DbgPrint("Unknown iocontrol\n");
		break;
	}

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;

}


NTSTATUS	InitIoComm(PDRIVER_OBJECT pDriverObj)
{
	UNICODE_STRING		DeviceName = RTL_CONSTANT_STRING(DEVICE_NAME);
	UNICODE_STRING		SymbolName = RTL_CONSTANT_STRING(SYMBOL_NAME);
	NTSTATUS			Status = STATUS_SUCCESS;


	Status = IoCreateDevice(pDriverObj,0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pDriverObj);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("IoCreateDevice failed:%x", Status);
		return Status;
	}

	//DO_BUFFERED_IO规定R3和R0之间read和write通信的方式：
	//1,buffered io
	//2,direct io
	//3,neither io
	//DO_DEVICE_INITIALIZING
	pDriverObj->Flags |= DO_BUFFERED_IO;


	Status = IoCreateSymbolicLink(&SymbolName, &DeviceName);
	if (!NT_SUCCESS(Status))
	{
		IoDeleteDevice(pDriverObj);
		DbgPrint("IoCreateSymbolicLink failed:%x\n", Status);
		return Status;
	}



	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION + 1; i++)
	{
		pDriverObj->MajorFunction[i] = DispatchCommon;
	}

	pDriverObj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoctrl;


	return Status;
}

