#pragma once
#include "util.h"

#define MUTEX_NAME TEXT("Mutex a funcionar?")
#define TAM 256


using namespace std;

class Partilha {
	int parede;
	int jogador;
	TCHAR monstro[TAM];
	HANDLE mutex;
public:

	Partilha();

	bool getParede();
	bool asJogador();
	bool asMonstro();

	void setParede();
	void setJogador(int i);
	void setMonstro(TCHAR m[TAM]);
	void setMutex(HANDLE m);

	int PartilhaMapa(int x, int y);

};


