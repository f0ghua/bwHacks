
#ifdef HOOKDLL_EXPORTS
#define HOOKDLL_API __declspec(dllexport)   // export DLL information
#else
#define HOOKDLL_API __declspec(dllimport)   // import DLL information
#endif 

HOOKDLL_API LONGLONG WINAPI version();
