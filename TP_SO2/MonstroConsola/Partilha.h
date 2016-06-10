#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <tchar.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <wchar.h>
#include <stdlib.h>
#include <time.h>

#define MUTEX_NAME TEXT("Mutex a funcionar?")
#define TAM 256

using namespace std;

class Partilha {
	int parede;
	int jogador;
	int monstro;
	HANDLE mutex;
public:

	Partilha();

	bool getParede();
	bool asJogador();
	bool asMonstro();

	void setParede();
	void setJogador(int j);
	void setMonstro(int m);
	void setMutex(HANDLE m);

	int PartilhaMapa(int x, int y);

};