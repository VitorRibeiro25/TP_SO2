#pragma once
#ifndef OBJ
#define OBJ

#include <iostream>
#include <vector>
#include <string>

#include "Jogador.h"

using namespace std;

class Objeto {
	string nome;
	int posX, posY;
	int vida;
	int lentidao; 	
public:
	Objeto(int x, int y);
	~Objeto();

	string getNome();
	int getPosX();
	int getPosY();
	int getVida();
	int getLentidao();

	void setNome(string n);
	void setPosX(int x);
	void setPosY(int y);
	void setVida(int v);
	void setLentidao(int l);
};

#endif