#pragma once
#ifndef MAPA
#define MAPA

#include <iostream>
#include <vector>
#include <string>

#include "Jogador.h"
#include "Monstro.h"
#include "Objeto.h"
#include "CelulaMapa.h"

using namespace std;

class Mapa {
	int pos; // se tiver 1 esta alguem na posicao, se estiver a 0 esta vazia 
	CelulaMapa *celula;
	int linhas, colunas;
	vector <Jogador *> jogs; // vetor de jogadores em jogo
	vector <Objeto *> obj; // vetor de objetos que estao espalhados pelo mapa
public:

	Mapa(int lin, int col);
	~Mapa();

	void predefinido();

	void NovoJogador(int x, int y);
	void AtualizaJogador(int x, int y);
	CelulaMapa &getCelula(int x, int y);
	
	int getLinhas();
	int getColunas();

};

#endif