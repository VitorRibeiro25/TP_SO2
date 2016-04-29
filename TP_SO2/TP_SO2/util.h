#ifndef UTIL
#define UTIL

#include <iostream>
#include <vector>
#include <string>
#include <tchar.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <wchar.h>


using namespace std;

#ifdef UNICODE
#define tstring wstring
#define tcout  wcout
#define tcerr wcerr
#define tcin wcin
#else
#define tstring string 
#define tcout cout
#define tcin cin
#define tcerr cerr
#endif


#endif