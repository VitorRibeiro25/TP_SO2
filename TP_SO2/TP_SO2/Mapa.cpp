#include "Mapa.h"

Mapa::Mapa(int lin, int col) {
	linhas = lin;
	colunas = col;

	celula = new CelulaMapa[lin * col];

	for (int i = 0; i < lin; i++) {
		for (int j = 0; j < col; j++) {
			if (i == 0 || j == 0 || i == lin || j == col) {
				getCelula(i, j).setParede(1);
			}
			else getCelula(i,j).setParede(0);
		}
	}

}

void Mapa::predefinido() {
	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {
			if ((i + j) % 5 == 0) {
				if (getCelula(i,j).getParede() == 0) {
					getCelula(i,j).setParede(1);
				}
			}
		}
	}

	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {
			if (getCelula(i,j).getParede() == 0) {
				NovoObjeto(i, j);
			}
		}
	}
}

void Mapa::random() {

	// ciclo para colocar paredes
	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {
		}
	}

	// ciclo para colocar objetos

	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {

		}
	}

}

void Mapa::NovoJogador(Jogador *jog) {

	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {
			if (getCelula(i,j).getParede() == 0) {
				jogs.push_back(jog);
			}
		}
	}
}

void Mapa::NovoObjeto(int x, int y) {

	int aux = rand() % 15;

	if (aux == 0) {
		ob = new Objeto(x, y);
		getCelula(x, y).setObjeto(1);
		obj.push_back(ob);
	}

}

void Mapa::ComeObjeto(Jogador *jog) {
	
	int x = jog->getPosX();
	int y = jog->getPosY();

	for (int i = 0; i < obj.size(); i++) {
		if (ob->getPosX() == x && ob->getPosY() == y) {
			// o jogador vai comer um objeto
			jog->ComeObjeto(ob);
			// incremnetar a pontuacao.
		}
	}

}

bool Mapa::VerificaParade(Jogador *jog) {
	
	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {
			if (jog->getPosX() == i && jog->getPosY() == j){
				if (getCelula(i, j).getParede() == 1) 
					return true;
				else return false;
			}
		}
	}
}

bool Mapa::VerificaObjeto(Jogador *jog) {
	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {
			if (jog->getPosX() == i && jog->getPosY() == j) {
				if (getCelula(i, j).getObjeto() == 1) {
					ComeObjeto(jog);
					return true;
				}
				else return false;
			}
		}
	}
}

CelulaMapa &Mapa::getCelula(int x, int y) {
	return celula[x * colunas + y];
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

