#include "Comm.h"



PDRIVER_OBJECT			g_pDriverObj = NULL;;
UNICODE_STRING			DeviceName = RTL_CONSTANT_STRING(DEVICE_NAME);
UNICODE_STRING			SymbolName = RTL_CONSTANT_STRING(SYMBOL_NAME);
BOOLEAN					DeviceAndSymbolLinkDelete = FALSE;


void		CommUnload() 
{
	if (!DeviceAndSymbolLinkDelete)
	{
		IoDeleteDevice(g_pDriverObj->DeviceObject);
		IoDeleteSymbolicLink(&SymbolName);
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

	ULONG					uIoctrlCode = 0;
	PVOID					pInputBuff = NULL;
	PVOID					pOutputBuff = NULL;

	ULONG					uInputLength = 0;
	ULONG					uOutputLength = 0;
	PIO_STACK_LOCATION		pStack =	NULL;
	NTSTATUS				Status =	STATUS_SUCCESS;
	ULONG_PTR				Info = 0;


	pInputBuff = pOutputBuff = pIrp->AssociatedIrp.SystemBuffer;

	pStack = IoGetCurrentIrpStackLocation(pIrp);
	uInputLength = pStack->Parameters.DeviceIoControl.InputBufferLength;
	uOutputLength = pStack->Parameters.DeviceIoControl.OutputBufferLength;
	uIoctrlCode = pStack->Parameters.DeviceIoControl.IoControlCode;


	switch (uIoctrlCode)
	{
		case CTL_GetFunListSize:
		{
			InfoOfSizeList InfoOfsize = { 0 };
			InfoOfsize.StructSize = sizeof(g_GetFunctionInfoList);
			InfoOfsize.ListCount = sizeof(g_GetFunctionInfoList) / sizeof(SymbolGetFunctionInfoList);
			memcpy(pOutputBuff, &InfoOfsize, sizeof(InfoOfsize));
			Info = sizeof(InfoOfsize);
			break;
		}
		case CTL_GetFunListInfo:
		{
			memcpy(pOutputBuff, &g_GetFunctionInfoList, sizeof(g_GetFunctionInfoList));
			Info = sizeof(g_GetFunctionInfoList);

			break;
		}
		case CTL_SendFunListInfo:
		{
			PSymbolGetFunctionInfoList		GetFunctionInfoList = (PSymbolGetFunctionInfoList)pInputBuff;
			ULONG	ListCount = sizeof(g_GetFunctionInfoList) / sizeof(SymbolGetFunctionInfoList);

			for (ULONG i = 0; i < ListCount; i++)
			{
				for (ULONG j = 0; j < Symbol_InfoListMax; j++)
				{
					if (strcmp(GetFunctionInfoList[i].InfoList[j].Name, Symbol_MaxListFlag) == 0)
					{
						break;
					}
					*g_GetFunctionInfoList[i].InfoList[j].ReceiveFunction = GetFunctionInfoList[i].InfoList[j].ReceiveFunction;
				}
			}
			break;
		}


		case CTL_GetTypeListSize:
		{
			InfoOfSizeList InfoOfsize = { 0 };
			InfoOfsize.StructSize = sizeof(g_GetTypeOffsetInfoList);
			InfoOfsize.ListCount = sizeof(g_GetTypeOffsetInfoList) / sizeof(SymbolGetTypeOffsetList);
			memcpy(pOutputBuff, &InfoOfsize, sizeof(InfoOfsize));
			Info = sizeof(InfoOfsize);
			break;
		}
		case CTL_GetTypeListInfo:
		{
			memcpy(pOutputBuff, &g_GetTypeOffsetInfoList, sizeof(g_GetTypeOffsetInfoList));
			Info = sizeof(g_GetTypeOffsetInfoList);
			break;
		}
		case CTL_SendTypeListInfo:
		{
			PSymbolGetTypeOffsetList		GetTypeInfoList = (PSymbolGetTypeOffsetList)pInputBuff;
			ULONG	ListCount = sizeof(g_GetTypeOffsetInfoList) / sizeof(SymbolGetTypeOffsetList);

			for (ULONG i = 0; i < ListCount; i++)
			{
				for (ULONG j = 0; j < Symbol_InfoListMax; j++)
				{
					if (strcmp(GetTypeInfoList[i].InfoList[j].ParentName, Symbol_MaxListFlag) == 0)
					{
						break;
					}

					*g_GetTypeOffsetInfoList[i].InfoList[j].Offset = (ULONG64)GetTypeInfoList[i].InfoList[j].Offset;
				}
			}
			break;
		}
		case  CTL_SymbolIsSuccess:
		{
			//抹掉IO通讯的痕迹
			CommUnload();

			for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION + 1; ++i)
				g_pDriverObj->MajorFunction[i] = NONO_IopInvalidDeviceRequest;

			DeviceAndSymbolLinkDelete = TRUE;

			//调用主函数
			RealDriverEntry();
			break;
		}
		default:
			DbgPrint("Unknown iocontrol\n");
			break;
	}


	pIrp->IoStatus.Status = Status;
	pIrp->IoStatus.Information = Info;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return Status;
}

NTSTATUS	InitIoComm(PDRIVER_OBJECT pDriverObj)
{

	NTSTATUS			Status = STATUS_SUCCESS;
	PDEVICE_OBJECT		pDeviceObject = NULL;
	OBJECT_ATTRIBUTES	 ObjectAttributes = {0};


	g_pDriverObj = pDriverObj;

	Status = IoCreateDevice(pDriverObj,0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pDeviceObject);

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
	pDeviceObject->Flags |= DO_BUFFERED_IO;

	Status = IoCreateSymbolicLink(&SymbolName, &DeviceName);
	if (!NT_SUCCESS(Status))
	{
		IoDeleteDevice(pDeviceObject);
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



