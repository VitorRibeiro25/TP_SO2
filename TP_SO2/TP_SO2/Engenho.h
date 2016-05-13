#pragma once
#ifndef ENGENHO_H
#define ENGENHO_H

#include <iostream>
#include <vector>
#include <string>
#include "util.h"
#include "Mapa.h"
#include "Jogador.h"

#define REGISTRY_KEY TEXT("Software\\TPSO2\\");

using namespace std;

class Engenho {
	bool jogoCriado;
	bool jogoIniciado;
	Mapa *m;
public:

	Engenho();
	void NovoRegisto(LPCWSTR nome, LPCWSTR pass);
	bool VerificaRegisto(LPCWSTR nome);
	int ExecutaComando(string tcomando, string comando);

	bool getJogoCriado();
	bool getJogoIniciado();
;

};

#endif 
