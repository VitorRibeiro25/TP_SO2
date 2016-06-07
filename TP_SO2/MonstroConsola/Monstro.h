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

using namespace std;

class Monstro {
	string nome;
	int vida;
	int id;
	static int id_m;
	int posX, posY;
	int lentidao;

public:

	Monstro(int x, int y);

	~Monstro();

	int getPosX();
	int getPosY();
	int getVida();
	int getLentidao();
	int getId();
	string getNome();

	void setPosX(int x);
	void setPosY(int y);
	void setVida(int vid);
	void setLentidao(int l);
	void setNome(string n);

	int movePosicao(Monstro *m);
	void combate(Monstro *m);

};
