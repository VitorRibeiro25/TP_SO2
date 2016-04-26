#pragma once

#include <iostream>
#include <string>
#include <tchar.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>

using namespace std;

#define MAXCLIENTES 5

//Esta macro é definida pelo sistema caso estejamos na DLL (<DLL_IMP>_EXPORTS definida)
//ou na app (<DLL_IMP>_EXPORTS não definida) onde DLL_IMP é o nome deste projecto
#ifdef GAMEDLL_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif


extern "C"
{
	//Variável global da DLL
	extern DLL_IMP_API  int PlayersActive;
	DLL_IMP_API void RemovePlayer();
	DLL_IMP_API int ShowPlayersActive();
	DLL_IMP_API void Imprime();

}
//DLL_IMP_API string TrataComando(TCHAR comando[256]);




