#pragma once

#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>

using namespace std;
#ifdef UNICODE 
#define tcout wcout
#define tcin wcin
#else
#define tcout cout
#define tcin cin
#endif

void Menu();
