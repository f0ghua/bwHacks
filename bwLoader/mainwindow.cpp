#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <windows.h>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_className = "SWarClass";
    AdjustPrivileges();
    InjectDll();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::AdjustPrivileges()
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    TOKEN_PRIVILEGES oldtp;
    DWORD dwSize = sizeof(TOKEN_PRIVILEGES);
    LUID luid;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        if (GetLastError()==ERROR_CALL_NOT_IMPLEMENTED) return true;
        else return false;
    }
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
        CloseHandle(hToken);
        return false;
    }
    ZeroMemory(&tp, sizeof(tp));
    tp.PrivilegeCount=1;
    tp.Privileges[0].Luid=luid;
    tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
    /* Adjust Token Privileges */
    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) {
        CloseHandle(hToken);
        return false;
    }
    // close handles
    CloseHandle(hToken);
    return true;
}

bool MainWindow::InjectDll()
{
    char *buffer = "f:\\game\\starcraft\\Plugins\\InjHlp.dll";

#if 0
    LPTSTR className = (LPTSTR)m_className.utf16();
    HWND windowHandle = FindWindow(className, NULL);
    if(windowHandle == NULL) {
        qDebug() << "Error: the specified window couldn't be found.\n";
        return false;
    }

    DWORD processID;
    DWORD actualProcId = GetWindowThreadProcessId(windowHandle, &processID);

    if (!actualProcId) {
        qDebug() << "Unable to find the process id !";
    }

    HANDLE process = GetProcessHandleFromHwnd(windowId);
    if (process == NULL) {
        qDebug() << "Error: the specified process couldn't be found.\n";
    }
#endif
    /*
    * Get process handle passing in the process ID.
    */
    int procID = 18260;
    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
    DWORD err = GetLastError();
    qDebug() << err;

    if (process == NULL) {
        qDebug() << "Error: the specified process couldn't be found.\n";
    }

    /*
    * Get address of the LoadLibrary function.
    */
    LPVOID addr = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
    if(addr == NULL) {
        qDebug() << "Error: the LoadLibraryA function was not found inside kernel32.dll library.\n";
    }

    /*
    * Allocate new memory region inside the process's address space.
    */
    LPVOID arg = (LPVOID)VirtualAllocEx(process, NULL, strlen(buffer), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (arg == NULL) {
        qDebug() << "Error: the memory could not be allocated inside the chosen process.\n";
    }

    /*
    * Write the argument to LoadLibraryA to the process's newly allocated memory region.
    */
    int n = WriteProcessMemory(process, arg, buffer, strlen(buffer), NULL);
    if (n == 0) {
        qDebug() << "Error: there was no bytes written to the process's address space.\n";
    }

    /*
    * Inject our DLL into the process's address space.
    */
    HANDLE threadID = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)addr, arg, NULL, NULL);
    if (threadID == NULL) {
        qDebug() << "Error: the remote thread could not be created.\n";
    } else {
        qDebug() << "Success: the remote thread was successfully created.\n";
    }

    /*
    * Close the handle to the process, becuase we've already injected the DLL.
    */
    CloseHandle(process);

    return true;
}
