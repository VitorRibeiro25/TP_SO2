#pragma once
#ifndef CELULA_H
#define CELULA_H

#include <iostream>
#include <vector>
#include <string>
#include "Objeto.h"
#include "Jogador.h"

using namespace std;

class CelulaMapa {
	
	Objeto *obj;
	int parede; // se estiver a 0 pode andar, se estiver a 1 é uma parede
	int porta; // se estiver a 0 nao tem porta, se esticer a 1 tem porta
	Jogador *jog;
public:

	CelulaMapa();

	int getParede();
	int getPorta();

	void setParede(int p);
	void setPorta(int p);

	void CriaObjeto(int x, int y);
	void ColocaJogador(int x, int y, int id);



};

#endif