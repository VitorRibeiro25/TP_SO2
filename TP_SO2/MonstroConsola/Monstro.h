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
#include "Partilha.h"

using namespace std;

class Monstro {
	TCHAR nome[TAM];
	int vida;
	int id;
	int linhas, colunas;
	HANDLE hMapFile;
	Partilha *p;
	static int id_m;
	int posX, posY;
	int lentidao;
	int n_casas;
public:

	Monstro(TCHAR nome[TAM], int linhas, int colunas);

	~Monstro();

	int getPosX();
	int getPosY();
	int getVida();
	int getId();
	int getLentidao();
	TCHAR getNome();
	Partilha getPartilha(int x, int y);

	void ColocaMonstro();

	void mostraInfo();

	void setPosX(int x);
	void setPosY(int y);
	void setVida(int vid);
	void setLentidao(int l);
	void setNome(TCHAR m[TAM]);

	int movePosicao(Monstro *m);
	void moveDist(int distancia);
	//int RandomDirecao();

};
