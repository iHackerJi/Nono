

#include "Public.h"
#include "symbol.h"
#include "comm.h"



int main() 
{

	BOOLEAN		Ret = FALSE;
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
		printf("发送信号");
		SendSuccessSignal();
	}
	if (hFile) CloseHandle(hFile);

	system("pause");

}