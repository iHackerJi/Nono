#include "Public.h"
#include "symbol.h"
#include "comm.h"


SC_HANDLE hService = NULL;

BOOLEAN	LoadDriver(char * ServiceName,char* DriverName)
{
	BOOLEAN bRet = FALSE;
	SC_HANDLE hSCManager = NULL;


	char	DriverPath[MAX_PATH] = {0};

	if (0 == GetCurrentDirectoryA(MAX_PATH, CurrentDirName))
	{
		printf("%s GetCurrentDirectoryA Error %d \r\n", __FUNCTION__, GetLastError());
		return	FALSE;
	}

	
	if (-1 == sprintf(DriverPath, "%s\\%s", CurrentDirName, DriverName))
	{
		printf("%s sprintf Error %d \r\n", __FUNCTION__, GetLastError());
		return FALSE;
	}

	hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCManager)
	{
		hService = CreateServiceA(hSCManager, ServiceName,
			ServiceName, SERVICE_ALL_ACCESS,
			SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START,
			SERVICE_ERROR_IGNORE, DriverPath,
			NULL, NULL, NULL, NULL, NULL
		);

		if (!hService)
		{
			hService = OpenServiceA(hSCManager, ServiceName, SERVICE_ALL_ACCESS);

			if (!hService)
			{
				CloseServiceHandle(hSCManager);
				return FALSE;
			}

		}

		bRet = StartServiceA(hService, 0, NULL);
		if (!bRet)
		{
			printf("%s StartServiceA Error %d \r\n", __FUNCTION__, GetLastError());
		}

	}


	if (hSCManager)	CloseServiceHandle(hSCManager);
	

	return bRet;
}

BOOLEAN	UnLoadDriver() {

	SERVICE_STATUS ServiceStatus = {0};
	if (!ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus))
	{
		printf("%s ControlService Error %d \r\n", __FUNCTION__, GetLastError());
		return	FALSE;
	}

	if (!DeleteService(hService))
	{
		printf("%s DeleteService Error %d \r\n", __FUNCTION__, GetLastError());
		return	FALSE;
	}

	printf("DriverUnload Success \r\n");
	return TRUE;

}


int main() 
{

	BOOLEAN		Ret = FALSE;
	char	ServiceName[] = "Nono";
	char	DriverName[] = "SuperNono.sys";

	if (!LoadDriver(ServiceName, DriverName))	goto _Exit;
	printf("Driverload Success \r\n");

	if (InitComm())
	{
		if (!InitSymbolFunctionList())
			goto _Exit;
		
		if (!InitSymbolTypeList())
			goto _Exit;
	}
	else 
	{
		goto _Exit;
	}
	
	Ret = TRUE;
_Exit:


	if (Ret)
	{
		SendSuccessSignal();
	}
	if (hFile)	CloseHandle(hFile);

	if (hService) 
	{
		UnLoadDriver();
		CloseServiceHandle(hService);
	}	


	system("pause");

}