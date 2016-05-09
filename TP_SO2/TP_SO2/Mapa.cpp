#include "Mapa.h"

Mapa::Mapa(int lin, int col) {
	linhas = lin;
	colunas = col;

	celula = new CelulaMapa*[lin];
	for (int i = 0; i < lin; i++) {
		celula[i] = new CelulaMapa[col];
	}

}


int Mapa::getLinhas() {
	return linhas;
}

int Mapa::getColunas() {
	return colunas;
}


Mapa::~Mapa() {

	delete[] celula;
}

