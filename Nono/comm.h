#pragma once
#include "Public.h"
#include "symbol.h"

HANDLE	hFile;

BOOLEAN	InitComm();
BOOLEAN	InitSymbolFunctionList();
BOOLEAN	InitSymbolTypeList();
void	DeleteMark();