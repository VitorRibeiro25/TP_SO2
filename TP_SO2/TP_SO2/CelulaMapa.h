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
	int objeto;
	Jogador *jog;
public:

	CelulaMapa();

	int getParede();
	int getPorta();
	int getObjeto();
	//int verificatudo();

	void setChao(int p);
	void setParede(int p);
	void setPorta(int p);
	void setObjeto(int o);

};

#endif