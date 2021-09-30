#pragma once
#include "../SymbolShare.h"

typedef void	(*ObKillProcessType)(
	PEPROCESS Process
);


PVOID					NONO_NtMapUserPhysicalPagesScatter;
PVOID					NONO_NtCallbackReturn;
PVOID					NONO_NtSuspendThread;
PVOID					NONO_IopInvalidDeviceRequest;
PVOID					NONO_NtUserGetThreadState;
PVOID					NONO_NtUserPeekMessage;
ObKillProcessType		NONO_ObKillProcess;


//���֧�� Symbol_InfoListMax ����ȡ����Ϣ

//����б���˿��Ի�ȡ����֮�⻹���Ի�ȡȫ�ֱ���
static	SymbolGetFunctionInfoList	g_GetFunctionInfoList[] =
{
	{
		"ntoskrnl.exe",
		{
			{"NtMapUserPhysicalPagesScatter",&NONO_NtMapUserPhysicalPagesScatter},
			{"NtCallbackReturn",&NONO_NtCallbackReturn},
			{"NtSuspendThread",&NONO_NtSuspendThread},
			{"IopInvalidDeviceRequest",&NONO_IopInvalidDeviceRequest},
			{"ObKillProcess",(PVOID*)&NONO_ObKillProcess},
			{Symbol_MaxListFlag,0}
		}
	},
	{
		"win32k.sys",
		{
			{"NtUserGetThreadState",&NONO_NtUserGetThreadState},
			{"NtUserPeekMessage",&NONO_NtUserPeekMessage},
			{Symbol_MaxListFlag,0}
		}
	}
};


ULONG64	CreateTime;
ULONG64	ThreadLock;
ULONG64	RundownProtect;

static	 SymbolGetTypeOffsetList	g_GetTypeOffsetInfoList[] =
{
	{
		"ntoskrnl.exe",
		{
			{"_ETHREAD","CreateTime",&CreateTime},
			{"_ETHREAD","ThreadLock",&ThreadLock},
			{"_ETHREAD","RundownProtect",&RundownProtect},
			{Symbol_MaxListFlag,Symbol_MaxListFlag,0}
		}
	}
};