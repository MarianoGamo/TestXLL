
#include <windows.h>
#include <xlcall.h>
#include <framewrk.h>
#include "UDFs.h"
 
#ifdef _DEBUG_PRINTF

///***************************************************************************
// debugPrintf()
//
// Purpose: 
//      sends a string to the debugger for the current application. 
//
// Parameters:
//
//      LPSTR lpFormat  The format definition string
//      ...             The values to print
//
// Returns: 
//
// Comments:
//
///***************************************************************************

void cdecl debugPrintf(LPSTR lpFormat, ...)
{
	char rgch[256];
	va_list argList;

	va_start(argList,lpFormat);
	wvsprintfA(rgch,lpFormat,argList);
	va_end(argList);
	OutputDebugStringA(rgch);
}

#endif /* DEBUG */
#ifndef _DEBUG_PRINTF
#define debugPrintf //
#endif

// Compares two strings for equality (case-insensitive).
int lpwstricmp(LPWSTR s, LPWSTR t)
{
       int i;
 
       if (wcslen(s) != *t)
       {
              return 1;
       }
 
       for (i = 1; i <= s[0]; i++)
       {
              if (towlower(s[i-1]) != towlower(t[i]))
              {
                     return 1;
              }
       }                                                                     
       return 0;
}
 
BOOL APIENTRY DllMain( HANDLE hDLL, DWORD dwReason, LPVOID lpReserved )
{
       switch (dwReason)
       {
       case DLL_PROCESS_ATTACH:
       case DLL_PROCESS_DETACH:
       case DLL_THREAD_ATTACH:
       case DLL_THREAD_DETACH:
       default:
              break;
       }
       return TRUE;
}
 
// Excel calls xlAutoOpen when it loads the XLL.
__declspec(dllexport) int WINAPI xlAutoOpen(void)
{
       static XLOPER12 xDLL;      // The filename of this XLL.
       int i;
 
       debugPrintf("xlAutoOpen\n");
 
       // Fetch the name of this XLL. This is used as the first arg
       // to the REGISTER function to specify the name of the XLL.
       Excel12f(xlGetName, &xDLL, 0);
       debugPrintf("XLL Name: %S\n", xDLL.val.str);
 
       // Loop through the g_rgUDFs[] table, registering each
       // function in the table using xlfRegister.
       for (i = 0; i < g_rgNumUDFs; i++)
       {
              Excel12f(xlfRegister, 0, 1 + g_rgUDFdata,
                       (LPXLOPER12) &xDLL,
                       (LPXLOPER12) TempStr12(g_rgUDFs[i][0]),
                       (LPXLOPER12) TempStr12(g_rgUDFs[i][1]),
                       (LPXLOPER12) TempStr12(g_rgUDFs[i][2]),
                       (LPXLOPER12) TempStr12(g_rgUDFs[i][3]),
                       (LPXLOPER12) TempStr12(g_rgUDFs[i][4]),
                       (LPXLOPER12) TempStr12(g_rgUDFs[i][5]),
                       (LPXLOPER12) TempStr12(g_rgUDFs[i][6]),
                       (LPXLOPER12) TempStr12(g_rgUDFs[i][7]),
                       (LPXLOPER12) TempStr12(g_rgUDFs[i][8]),
                       (LPXLOPER12) TempStr12(g_rgUDFs[i][9]),
                       (LPXLOPER12) TempStr12(g_rgUDFs[i][10])
                       );
       }
 
       // Free the XLL filename.
       Excel12f(xlFree, 0, 1, (LPXLOPER12) &xDLL);
      return 1;
}
 
// Excel calls xlAutoClose when it unloads the XLL.
__declspec(dllexport) int WINAPI xlAutoClose(void)
{
       int i;
       debugPrintf("xlAutoClose\n");
 
       // Delete all names added by xlAutoOpen or xlAutoRegister.
       for (i = 0; i < g_rgNumUDFs; i++)
       {
              Excel12f(xlfSetName, 0, 1, TempStr12(g_rgUDFs[i][2]));
       }
 
       return 1;
}
 
// Excel calls xlAutoRegister12 if a macro sheet tries to register
// a function without specifying the type_text argument.
__declspec(dllexport) LPXLOPER12 WINAPI xlAutoRegister12(LPXLOPER12 pxName)
{
       static XLOPER12 xDLL, xRegId;
       int i;
 
       debugPrintf("xlAutoRegister12\n");
       xRegId.xltype = xltypeErr;
       xRegId.val.err = xlerrValue;
 
       for (i = 0; i < g_rgNumUDFs; i++)
       {
              if (!lpwstricmp(g_rgUDFs[i][0], pxName->val.str))
              {
                     Excel12f(xlfRegister, 0, 1 + g_rgUDFdata,
                             (LPXLOPER12) &xDLL,
                             (LPXLOPER12) TempStr12(g_rgUDFs[i][0]),
                             (LPXLOPER12) TempStr12(g_rgUDFs[i][1]),
                             (LPXLOPER12) TempStr12(g_rgUDFs[i][2]),
                             (LPXLOPER12) TempStr12(g_rgUDFs[i][3]),
                             (LPXLOPER12) TempStr12(g_rgUDFs[i][4]),
                             (LPXLOPER12) TempStr12(g_rgUDFs[i][5]),
                             (LPXLOPER12) TempStr12(g_rgUDFs[i][6]),
                             (LPXLOPER12) TempStr12(g_rgUDFs[i][7]),
                             (LPXLOPER12) TempStr12(g_rgUDFs[i][8]),
                             (LPXLOPER12) TempStr12(g_rgUDFs[i][9]),
                             (LPXLOPER12) TempStr12(g_rgUDFs[i][10])
                             );
 
                     // Free the oper returned by Excel.
                     Excel12f(xlFree, 0, 1, (LPXLOPER12) &xDLL);
                     return(LPXLOPER12) &xRegId;
              }
       }
 
       return(LPXLOPER12) &xRegId;
}
 
// When you add an XLL to the list of active add-ins, the Add-in
// Manager calls xlAutoAdd() and then opens the XLL, which in turn
// calls xlAutoOpen.
__declspec(dllexport) int WINAPI xlAutoAdd(void)
{
       debugPrintf("xlAutoAdd\n");
       return 1;
}
 
// When you remove an XLL from the list of active add-ins, the
// Add-in Manager calls xlAutoRemove() and then
// UNREGISTER("Funciones_de_Mariano.XLL").
__declspec(dllexport) int WINAPI xlAutoRemove(void)
{
       debugPrintf("xlAutoRemove\n");
       return 1;
}
 
// The Excel Add-in Manager calls xlAddInManagerInfo12 function
// to find the long name of the add-in.
__declspec(dllexport) LPXLOPER12 WINAPI xlAddInManagerInfo12(LPXLOPER12 xAction)
{
       static XLOPER12 xInfo, xIntAction;
 
       debugPrintf("xlAutoAddInManagerInfo12\n");
 
       // This code coerces the passed-in value to an integer.
       Excel12f(xlCoerce, &xIntAction, 2, xAction,
              TempInt12(xltypeInt));
 
       if (xIntAction.val.w == 1)
       {
             // Note that the string is length-prefixed in octal.
              xInfo.xltype = xltypeStr;
              xInfo.val.str = L"\016Funciones_de_Mariano";
       }
       else
       {
              xInfo.xltype = xltypeErr;
              xInfo.val.err = xlerrValue;
       }
 
       // Word of caution: returning static XLOPERs/XLOPER12s is
       // not thread-safe. For UDFs declared as thread-safe, use
       // alternate memory allocation mechanisms.
       return(LPXLOPER12) &xInfo;
}
