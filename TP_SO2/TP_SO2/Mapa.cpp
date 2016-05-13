#include "Mapa.h"

Mapa::Mapa(int lin, int col) {
	linhas = lin;
	colunas = col;

	celula = new CelulaMapa*[lin];
	for (int i = 0; i <= lin; i++) {
		celula[i] = new CelulaMapa[col];
		for (int j = 0; j <= col; j++) {
			if (i == 0 || j == 0 || i == lin || j == col) {
				celula[i][j].setParede(1);
			}
			else celula[i][j].setParede(0);
		}
	}

}

void Mapa::predefinido() {
	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {
			if ((i + j) % 2 == 0) {
				if (celula[i][j].getParede() == 0) {
					celula[i][j].setParede(1);
				}
			}
		}
	}

	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {
			if (celula[i][j].getParede() == 0) {
				celula[i][j].CriaObjeto(i,j);
			}
		}
	}
}

void Mapa::NovoJogador(int x, int y) {

	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {
			if (celula[x][y].getParede() == 0) {
				celula[i][j].ColocaJogador(x, y);
			}
		}
	}
}

void Mapa::AtualizaJogador(int x, int y) {

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

