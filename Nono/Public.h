#pragma once
#include <stdio.h>
#include <windows.h>
#include <shlwapi.h>
#include <subauth.h>
#pragma comment(lib, "Shlwapi.lib")


HANDLE		hProcess;
char		CurrentDirName[MAX_PATH];