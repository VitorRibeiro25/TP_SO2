#pragma once
#ifndef MAPA
#define MAPA

#include <iostream>
#include <vector>
#include <string>
#include "Jogador.h"
#include "Monstro.h"
#include "Objeto.h"

using namespace std;



class Mapa {
	int pos; // se tiver 1 esta alguem na posicao, se estiver a 0 esta vazia 
	vector <Jogador *> jogs; // vetor de jogadores em jogo
	vector <Monstro *> monst; // vetor de monstro espalhados pelo mapa
	vector <Objeto *> obj; // vetor de objetos que estao espalhados pelo mapa
public:
};

#endif