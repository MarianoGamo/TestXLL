#ifndef UDFS_H
#define UDFS_H

#include <vector>
#include <windows.h>
#include <xlcall.h>
#include <framewrk.h>
#include <stdio.h>

#define g_rgNumUDFs 2
#define g_rgUDFdata 11


static LPWSTR g_rgUDFs
[g_rgNumUDFs][g_rgUDFdata] =
{

		{
              L"Multiplicate",                 // Function name/ordinal
              L"JJJ",                    // Func signature type
              L"Multiplicate",                 // Func name in Func wizard
              L"for, acumulador",             // Arg name in Func wizard
              L"1",                      // Function type
              L"Funciones_de_Mariano",           // Category in Func wizard
              L"",                       // Shortcut (commands only)
              L"",                       // Help topic
              L"SumTwo function help",  // Func help in Func wizard
              L"Un numerito, con el numero de iteraciones",          // Arg help in Func wizard
              L"cantidad a acumular"           // Arg help in Func wizard       

       },
      
	   {
		   L"MultiplicateArray",
		   L"UU",
		   L"MultiplicateArray",
		   L"La funcion espera un rango de dos columnas",
		   L"1",
		   L"Funciones_de_Mariano",
		   L"",
		   L"",
		   L"MultiplicateArray function help",
		   L"",
		   L"cantidades a multiplicar"
	   }
};

long compute(long arg1, long arg2);


#endif