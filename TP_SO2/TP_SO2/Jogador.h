#pragma once
#ifndef JOG
#define JOG

#include <iostream>
#include <vector>
#include <string>
#include "util.h"

#include "Objeto.h"

class Objeto;

using namespace std;

class Jogador {
	static int id_inc;
	int id;
	string nome;
	bool emJogo; // se esitver a 1 esta a jogar, caso contrario esta apenas logado
	int lentidao;
	int vida;
	int posX, posY;
	bool pedra;
	vector <Objeto *> obj;
public:

	Jogador(int id);
	~Jogador();

	int getId();
	string getNome();
	int getVida();
	int getLentidao();
	int getPosX();
	int getPosY();
	bool getPedra();

	void ComeObjeto(Objeto *ob);
	bool adicionaPedra(Objeto *ob);
	bool verificaPedra();
	void retiraPedra();
	string UltimoObjetos();

	void setNome(string na);
	void setVida(int v);
	void setLentidao(int l);
	void setPosX(int x);
	void setPosY(int y);
	void setPedra(bool p);

};

#endif