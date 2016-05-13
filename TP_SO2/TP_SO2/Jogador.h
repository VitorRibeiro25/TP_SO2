#pragma once
#ifndef JOG
#define JOG

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Objeto;

class Jogador {
	static int id_inc;
	int id;
	string nome;
	bool emJogo; // se esitver a 1 esta a jogar, caso contrario esta apenas logado
	int lentidao;
	int vida;
	int posX, posY;
	vector <Objeto *> obj;
public:

	Jogador(int x, int y);
	~Jogador();

	int getId();
	string getNome();
	int getVida();
	int getLentidao();
	int getPosX();
	int getPosY();


	void setNome(string na);
	void setVida(int v);
	void setLentidao(int l);
	void setPosX(int x);
	void setPosY(int y);

};

#endif