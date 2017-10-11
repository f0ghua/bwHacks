// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "stdio.h"

#define _T(x) x

FILETIME g_ftModify = {0};

// 定时器回调函数  
void CALLBACK TimeProc(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime);  
// 线程回调函数  
DWORD CALLBACK ThreadProc(PVOID pvoid);

static int printFile(char *fmt, ...)  
{
#if 0
	va_list	val;
    FILE *file;
    fopen_s(&file, "c:\\log_repHelper.txt", "a+");

	va_start(val, fmt);
	int n = vfprintf(file, fmt, val);
	va_end(val);

	fclose(file);
	return n;
#endif
	return 0;
}


static int GetFileModifyTime(LPCSTR strFilePath, FILETIME *ftModify)
{
	FILETIME ftCreate, ftAccess;

	HANDLE hFile = CreateFile(strFilePath, GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                            // default security
		OPEN_EXISTING,          // existing file only
		FILE_FLAG_BACKUP_SEMANTICS , // normal file
		NULL);
	
	if (!GetFileTime(hFile, &ftCreate, &ftAccess, ftModify)) {
		return -1;
	}

	return 0;
}

static int createDirectoryRecursively( LPSTR path )
{
	char folder[MAX_PATH];
	char *end;
	ZeroMemory(folder, MAX_PATH * sizeof(char));

	// skip the first dir (maybe f:\\)
	end = strchr(path, '\\');
	if (end != NULL) end = strchr(++end, '\\');

	while(end != NULL) {
		strncpy_s(folder, path, end - path + 1);
		//printFile("folder = %s\n", folder);
		if(!CreateDirectory(folder, NULL)) {
			DWORD err = GetLastError();

			if(err != ERROR_ALREADY_EXISTS) {
				// do whatever handling you'd like
				printFile("create dir err = %d, folder = %s\n", err, folder);
				return -1;
			}
		}
		end = strchr(++end, '\\');
	}

	CreateDirectory(path, NULL);

	return 0;
}

static void UpdateRepFiles()
{
	FILETIME ftModify;
	char repPath[128];
	char repDir[128];
	LPCSTR scPath = _T("f:\\game\\starcraft");
	LPCSTR repSubDir = _T("maps\\replays\\autoReplay");
	LPCSTR tmpFile = _T("maps\\replays\\LastReplay.rep");
	char tmpPath[128];
	sprintf_s(tmpPath, "%s\\%s", scPath, tmpFile);
	printFile("%s\n", tmpPath);

	if (GetFileModifyTime(tmpPath, &ftModify) == -1) {
		return;
	}

	if (CompareFileTime(&ftModify, &g_ftModify) == 0) {
		printFile("time is same, do nothing\n");
		return;
	}
	SYSTEMTIME stLocal;
	ZeroMemory(&stLocal, sizeof(SYSTEMTIME));
	FileTimeToSystemTime(&ftModify, &stLocal);
	printFile("%04d-%02d-%02d %02d:%02d:%02d\n", 
			stLocal.wYear, stLocal.wMonth, stLocal.wDay,  stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
	char subdir[128];
	sprintf_s(subdir, "%02d%02d%02d", stLocal.wYear%100, stLocal.wMonth, stLocal.wDay);
	sprintf_s(repDir, "%s\\%s\\%s", scPath, repSubDir, subdir);
	sprintf_s(repPath, "%s\\%04d%02d%02d%02d%02d%02d.rep", repDir,
		stLocal.wYear, stLocal.wMonth, stLocal.wDay,  stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
	printFile("%s %s\n", repDir, repPath);
	createDirectoryRecursively(repDir);
	if (CopyFile(tmpPath, repPath, true) == false) {
		printFile("copy file from %s to %s fail\n", tmpPath, repPath);
	}

	g_ftModify = ftModify;
}

void CALLBACK TimeProc(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)  
{  
	UpdateRepFiles();
}  
  
DWORD CALLBACK ThreadProc(PVOID pvoid)  
{  
    MSG msg;  
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);   
    SetTimer(NULL, 10, 60*1000, TimeProc);  
    while(GetMessage(&msg, NULL, 0, 0))  
    {  
        if(msg.message == WM_TIMER)  
        {  
            TranslateMessage(&msg);    // 翻译消息  
            DispatchMessage(&msg);     // 分发消息  
        }  
    }  
    KillTimer(NULL, 10);  
    return 0;  
}  

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DWORD dwThreadId;
		// 创建线程  
		HANDLE hThread = CreateThread(NULL, 0, ThreadProc, 0, 0, &dwThreadId); 
		printFile("repHelper thread start\n");
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

