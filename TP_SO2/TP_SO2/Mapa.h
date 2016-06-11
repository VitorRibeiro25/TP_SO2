#pragma once
#ifndef MAPA
#define MAPA

#include <iostream>
#include <vector>
#include <string>

#include "Jogador.h"
#include "Objeto.h"
#include "CelulaMapa.h"
#include "Partilha.h"

using namespace std;

class Mapa {
	int pos; // se tiver 1 esta alguem na posicao, se estiver a 0 esta vazia 
	CelulaMapa *celula;
	Objeto *ob;
	Partilha *p;
	int linhas, colunas;
	vector <Jogador *> jogs; // vetor de jogadores em jogo
	vector <Objeto *> obj; // vetor de objetos que estao espalhados pelo mapa
public:

	Mapa(int lin, int col);
	~Mapa();

	void predefinido();
	void random();

	void NovoJogador(Jogador *jog);
	void NovoObjeto(int x, int y);

	void ComeObjeto(Jogador *jog);
	bool verificaVida(Jogador *jog);
	bool VerificaParade(Jogador *jog);
	bool VerificaParede(int x, int y);
	bool VerificaObjeto(Jogador *jog);
	bool VerificaObjetos(Jogador *jog);
	int verifaObjetoNome(int x, int y);
	bool VerificaJogadores(Jogador *jog);
	int VerificaJogadoresXY(int x, int y);
	int VerificaAdjacencia(Jogador *jog);
	bool VerificaJogador(Jogador *jog);


	Jogador *GuardaJogador(Jogador *jog);
	void Combate(Jogador *jog);

	CelulaMapa &getCelula(int x, int y);
	
	int getLinhas();
	int getColunas();

};

#endif