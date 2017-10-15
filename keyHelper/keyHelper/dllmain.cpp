// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <string>
#include "resource.h"

#define G_MY_WND_CLASS_NAME L"my wnd class name"
#define G_MY_WND_TITLE_NAME L"my wnd title name"

HMODULE g_hModuleDll = NULL; ///< ��DLL������ʱ��Handle, ���Handel�������Լ���, ������������handle
HWND    g_hMsgWnd = NULL; ///< �Լ���������Ϣ���ھ��
HHOOK   g_hHookMouse = NULL; ///< ����ע��ĳ�����consoleʱ��û�������Ϣ
HHOOK   g_hHookKeyboard = NULL;
BOOL g_bNeedSkipHookProc = FALSE;

/// @fn     DP
/// @brief  ��ӡ������Ϣ
void DbgPrtW(wchar_t *fmt,...);
#ifdef _DEBUG 
#define DP(fmt, ...) DbgPrtW(fmt, ##__VA_ARGS__)
#else
#define DP(fmt, ...)
#endif
BOOL GetFilePathName_host(std::wstring & strPathName);
BOOL IsNeedSkipHook();

/// @fn     Thread_CreateMyWnd
/// @brief  �߳�, ����DLL�Լ��Ĵ���, Ϊ��ʹDLLӵ����Ϣѭ��
UINT Thread_CreateMyWnd(LPVOID lParam);

/// @fn     CreateMyWindow
/// @brief  �����Լ��Ĵ���
BOOL CreateMyWindow(
    HINSTANCE hInstance, 
    int nCmdShow, 
    wchar_t * lpcWndClass, 
    wchar_t * lpcWndTitleName, 
    HWND & hWndParent);

ATOM MyRegisterClass(HINSTANCE hInstance, wchar_t * pcClassName);
LRESULT CALLBACK WindowProc(      
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam);

void SetWndHook();
void UnSetWndHook();

void SetWndHook_Mouse();
LRESULT CALLBACK cbProcMouse(int iCode, WPARAM wParam, LPARAM lParam);

void SetWndHook_Keyboard();
LRESULT CALLBACK cbProcKeyboard(int iCode, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    HANDLE  hThread = INVALID_HANDLE_VALUE;
    DWORD   nThreadId = 0;

    switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
        {
            g_hModuleDll = hModule;
            g_bNeedSkipHookProc = IsNeedSkipHook();

            /// DLL������ʱ, �ʹ����Լ��Ĵ�����Ϣ����
            /// �ɴ�����Ϣ��������, ���Hook������
            DP(L">> DllHook.dll DLL_PROCESS_ATTACH");
            hThread = ::CreateThread(
                NULL, 
                0, 
                (LPTHREAD_START_ROUTINE)Thread_CreateMyWnd, 
                0, 
                0, 
                &nThreadId);
            CloseHandle(hThread);
        }
        break;

	case DLL_THREAD_ATTACH:
        break;

	case DLL_THREAD_DETACH:
        break;

	case DLL_PROCESS_DETACH:
        {
            /// �ô�����Ȼ��ʧ, �����UnHook
            if (NULL != g_hMsgWnd)
                SendMessage(g_hMsgWnd, WM_CLOSE, 0, 0);

            DP(L">> DllHook.dll DLL_PROCESS_DETACH");
        }
		break;

    default:
        break;
	}

	return TRUE;
}

void DbgPrtW(wchar_t *fmt,...)
{
    va_list marker;
    wchar_t szBufW[4096];

    do 
    {
        if (g_bNeedSkipHookProc)
            break;

        ::ZeroMemory(szBufW, sizeof(szBufW));

        va_start(marker, fmt);
        wvsprintfW(szBufW, fmt, marker);
        va_end(marker);

        OutputDebugStringW(szBufW);
    } while (0);
}

UINT Thread_CreateMyWnd(LPVOID lParam)
{
    BOOL        bRc = FALSE;
    HWND        hWndParent = NULL;
    HMODULE     hModule = NULL;
    ATOM        atom;
    MSG         msg;
    HACCEL      hAccelTable = NULL;
    HINSTANCE   hInst = NULL;

    UNREFERENCED_PARAMETER(lParam);
    hWndParent = ::GetConsoleWindow();

    hModule = ::GetModuleHandle(NULL);
    atom = MyRegisterClass(hModule, G_MY_WND_CLASS_NAME);
    if (0 == atom)
    {
        DP(L"ERROR : MyRegisterClass\n");
    }

    /// ����ʵ��������һ��Ҫ�ɹ�, �������ⲻͣ�ĳ��Դ�������
    /// ���ڱ�������ԴID(accelerator, icon, menu), ���ܴ����ɹ�
    do
    {
        bRc = CreateMyWindow(
            hModule,
            SW_HIDE, 
            G_MY_WND_CLASS_NAME, 
            G_MY_WND_TITLE_NAME, 
            hWndParent);

        DP(L"CreateMyWindow [%s]\n", bRc ? L"TRUE" : L"FALSE");
        Sleep(1000);
    } while (!bRc);

    hAccelTable = LoadAccelerators(
        hModule, 
        MAKEINTRESOURCE(IDR_ACCELERATOR_XX));

    DP(L">> message loop");
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    DP(L"<< Thread_CreateMyWnd, msg.wParam = 0x%x", msg.wParam);
    return (int) msg.wParam;
}

BOOL CreateMyWindow(
    HINSTANCE hInstance,
    int nCmdShow,
    wchar_t * pcWndClass,
    wchar_t * pcWndTitleName,
    HWND & hWndParent)
{
    g_hMsgWnd = CreateWindowW(
        pcWndClass,
        pcWndTitleName,
        WS_OVERLAPPEDWINDOW,
        0,
        0,
        0,
        0,
        hWndParent,
        NULL, 
        hInstance, 
        NULL);
    if (NULL == g_hMsgWnd)
    {
        DP(L"error : CreateWindowW\r\n");
        return FALSE;
    }

    ShowWindow(g_hMsgWnd, nCmdShow);
    UpdateWindow(g_hMsgWnd);

    DP(L"OK : CreateWindowW\r\n");
    return TRUE;
}

ATOM MyRegisterClass(HINSTANCE hInstance, wchar_t * pcClassName)
{
    WNDCLASSEXW	wcex;

    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WindowProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_XX));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDR_MENU_XX);
    wcex.lpszClassName  = pcClassName;
    wcex.hIconSm        = 
        LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON_XX));

    return RegisterClassExW(&wcex);
}

LRESULT CALLBACK WindowProc(
    HWND hWnd, 
    UINT message, 
    WPARAM wParam, 
    LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_CREATE:
        SetWndHook();   ///< ��Ϣѭ���Ѿ�����, ����SetWindowsHookEx
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code here...
        EndPaint(hWnd, &ps);
        break;

        /// ���ڹر�ʱ, WM_CLOSE >> WM_DESTROY
    case WM_CLOSE:
        DP(L">> WM_CLOSE\r\n");

    case WM_DESTROY:
        DP(L">> WM_DESTROY\r\n");
        UnSetWndHook();///< ����UnHook, �ڴ��屻����֮ǰ
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

void SetWndHook()
{
    std::wstring strPathName = L"";

    do 
    {
#if 0
        if (NULL == g_hHookMouse)
            SetWndHook_Mouse();
#endif
        /// Set Other Wnd Hook

        if (NULL == g_hHookKeyboard)
            SetWndHook_Keyboard();
    } while (0);
}

void UnSetWndHook()
{
    if (NULL != g_hHookMouse)
    {
        DP(L"UnhookWindowsHookEx(0x%p)\r\n", g_hHookMouse);
        UnhookWindowsHookEx(g_hHookMouse);
        g_hHookMouse = NULL;
    }
    else
    {
        DP(L"NULL == g_hHookMouse, need not unHook\r\n");
    }

    /// UnSet Other Wnd Hook
    if (NULL != g_hHookKeyboard)
    {
        DP(L"UnhookWindowsHookEx(0x%p)\r\n", g_hHookKeyboard);
        UnhookWindowsHookEx(g_hHookKeyboard);
        g_hHookKeyboard = NULL;
    }
    else
    {
        DP(L"NULL == g_hHookKeyboard, need not unHook\r\n");
    }
}

void SetWndHook_Mouse()
{
    DWORD dwTid = 0;
    DWORD dwID = 0;
    HWND hIEModule = NULL;
    DWORD dwErr = 0;

    hIEModule = (HWND)GetModuleHandle(NULL);
    DP(L"[hIEModule] hIEModule is:0x%x \n",hIEModule);
    dwTid = GetWindowThreadProcessId(hIEModule,&dwID);

    if (NULL == g_hHookMouse)
    {
        /// SetWindowsHookEx �Դ��ڳ������Ч��, ����ǿ���̨���� ��Ч��
        /// ��õ� ERROR_HOOK_NEEDS_HMOD ����
        g_hHookMouse = SetWindowsHookEx(
            WH_MOUSE,
            (HOOKPROC)cbProcMouse,
            g_hModuleDll/*GetModuleHandle(NULL)*/,
            dwTid);

        dwErr = GetLastError();

        // WH_KEYBOARD

        DP(L"[g_hHookMouse] g_hHookMouse = 0x%p\r\n", g_hHookMouse);
        if (NULL == g_hHookMouse)
        {
            // ERROR_HOOK_NEEDS_HMOD
            DP(L"g_hHookMouse SetWindowsHookEx LastError = %d", dwErr);
        }
    }
}

void SetWndHook_Keyboard()
{
    DWORD dwTid = 0;
    DWORD dwID = 0;
    HWND hIEModule = NULL;
    DWORD dwErr = 0;

    hIEModule = (HWND)GetModuleHandle(NULL);
    DP(L"[hIEModule] hIEModule is:0x%x \n",hIEModule);
    dwTid = GetWindowThreadProcessId(hIEModule,&dwID);

    if (NULL == g_hHookKeyboard)
    {
        /// SetWindowsHookEx �Դ��ڳ������Ч��, ����ǿ���̨���� ��Ч��
        /// ��õ� ERROR_HOOK_NEEDS_HMOD ����
        g_hHookKeyboard = SetWindowsHookEx(
            WH_KEYBOARD_LL,
            (HOOKPROC)cbProcKeyboard,
            g_hModuleDll /*GetModuleHandle(NULL)*/,
            0 /*dwTid*/);
        dwErr = GetLastError();

        DP(L"[g_hHookKeyboard] g_hHookKeyboard = 0x%p\r\n", g_hHookKeyboard);
        if (NULL == g_hHookKeyboard)
        {
            // ERROR_HOOK_NEEDS_HMOD
            DP(L"g_hHookKeyboard SetWindowsHookEx LastError = %d", dwErr);
        }
    }
}

LRESULT CALLBACK cbProcMouse(int iCode, WPARAM wParam, LPARAM lParam)
{
    do 
    {
        if (g_bNeedSkipHookProc)
            break;

        if (wParam == WM_LBUTTONDOWN) {
            DP(L"WM_LBUTTONDOWN hit~\r\n");
		}

    } while (0);

    return CallNextHookEx(g_hHookMouse, iCode, wParam, lParam);
}

LRESULT CALLBACK cbProcKeyboard(int iCode, WPARAM wParam, LPARAM lParam)
{
    do 
    {
        if (g_bNeedSkipHookProc)
            break;
		DP(L"cbProcKeyboard: iCode = %d, vCode = %d\r\n",iCode, ((PKBDLLHOOKSTRUCT) lParam)->vkCode);
        if (HC_ACTION == iCode)
        {
			DWORD keyCode = ((PKBDLLHOOKSTRUCT) lParam)->vkCode;
			if ((keyCode == VK_LWIN) || (keyCode == VK_RWIN)) {
				return 1;
			}

			/*
            if (wParam == VK_SPACE)
            {
                DP(L"VK_SPACE was pressed~\r\n");
            }
			*/
        }
    } while (0);

    return CallNextHookEx(g_hHookKeyboard, iCode, wParam, lParam);
}

BOOL GetFilePathName_host(std::wstring & strPathName)
{
    WCHAR szModuleMe[MAX_PATH + 1] = { L'\0'};

    ::GetModuleFileName(NULL, szModuleMe, MAX_PATH);
    strPathName = szModuleMe;

    return TRUE;
}

BOOL IsNeedSkipHook()
{
    BOOL bRc = TRUE;
    std::wstring strPathName = L"";

    do 
    {
        if (!GetFilePathName_host(strPathName))
            break;

        // Dbgview.exe devenv.exe
        // �Ź�������, Ҫ�����涼�ҵ���
        if ((std::wstring::npos != strPathName.find(L"Dbgview.exe"))
            || (std::wstring::npos != strPathName.find(L"devenv.exe")))
        {
            break;
        }

        bRc = FALSE; ///< ���HookDll���������ǵ���������, �Ͳ���Ҫ����Hook�Ĵ���
    } while (0);

    return bRc;
}