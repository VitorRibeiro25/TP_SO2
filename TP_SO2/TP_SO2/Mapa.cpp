#include "Mapa.h"

Mapa::Mapa(int lin, int col) {

	linhas = lin;
	colunas = col;

	celula = new CelulaMapa[lin * col];

	for (int i = 0; i <= lin; i++) {
		for (int j = 0; j <= col; j++) {
			if (i == 0 || j == 0 || i == lin || j == col) {
				getCelula(i, j).setParede(1);
			}
			else getCelula(i,j).setParede(0);
		}
	}

}


void Mapa::predefinido() {
	/*
	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {
			if ((i + j) % 5 == 0) {
				if (getCelula(i,j).getParede() == 0) {
					getCelula(i,j).setParede(1);
				}
			}
		}
	}
	*/
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

	int aux = rand() % 5;

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
		if (obj[i]->getPosX() == x && obj[i]->getPosY() == y) {
			// o jogador vai comer um objeto
			jog->ComeObjeto(obj[i]);

			// colocar a celula com o objeto a 0
			getCelula(obj[i]->getPosX(), obj[i]->getPosY()).setObjeto(0);

			// remover objeto do vetor de objetos do mapa
			// este erase da cabo de todo o jogo
			obj.erase(obj.begin() + i);
		}
	}

}

bool Mapa::verificaVida(Jogador *jog) {

	int x = jog->getPosX();
	int y = jog->getPosY();

	if (jog->getVida() > 0) {
		return true;
	}
	else {
		for (int i = 0; i < jogs.size(); i++) {
			if (jogs[i]->getPosX() == x && jogs[i]->getPosY() == y) {
				jogs.erase(jogs.begin() + i);
			}
		}
		return false;
	}

}

bool Mapa::VerificaParade(Jogador *jog) {
	
	for (int i = 0; i <= linhas; i++) {
		for (int j = 0; j <= colunas; j++) {
			if (jog->getPosX() == i && jog->getPosY() == j){
				if (getCelula(i, j).getParede() == 1) 
					return true;
				else return false;
			}
		}
	}
}

bool Mapa::VerificaParede(int x, int y) {
	for (int i = 0; i <= linhas; i++) {
		for (int j = 0; j <= colunas; j++) {
			if (x == i && y == j) {
				if (getCelula(i, j).getParede() == 1)
					return true;
				else return false;
			}
		}
	}
}

int Mapa::Verificacelula(int x, int y) {

	bool parede, objeto, jogador;

	if (x < 0 || y < 0 || x > linhas || y > colunas) {
		return 9; //fora do mapa
	}
	else {

		parede = VerificaParede(x, y);
		objeto = VerificaObjetosXY(x, y);
		jogador = VerificaJogadoresXY(x, y);

		if (parede == true) {
			return 1; // parede é 1 
		}
		else if (objeto == true) {
			return 2; //objeto é 2
		}
		else if (jogador == true) {
			return 3; //jogador é 3
		}
		else {
			return 0; //É so chão
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

bool Mapa::VerificaObjetos(Jogador *jog) {
	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {
			if (jog->getPosX() == i && jog->getPosY() == j) {
				if (getCelula(i, j).getObjeto() == 1) {
					return true;
				}
				else return false;
			}
		}
	}
}

bool Mapa::VerificaObjetosXY(int x, int y) {
	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {
			if (x == i && y== j) {
				if (getCelula(i, j).getObjeto() == 1) {
					return true;
				}
				else return false;
			}
		}
	}
}

int Mapa::VerificaAdjacencia(Jogador *jog) {
	int x = jog->getPosX();
	int y = jog->getPosY();


	for (int h = 0; h < jogs.size(); h++) {
		// jogador baixo
		if (jogs[h]->getPosX() == x + 1 && jogs[h]->getPosY() == y) {
			return 1;
		}
		// jogador cima
		else if (jogs[h]->getPosX() == x - 1 && jogs[h]->getPosY() == y) {
			return 2;
		}
		// jogador a direita
		else if (jogs[h]->getPosX() == x && jogs[h]->getPosY() == y + 1) {
			return 3;
		}
		// jogador esquerda
		else if (jogs[h]->getPosX() == x && jogs[h]->getPosY() == y - 1) {
			return 4;
		}
		// nao existe jogador em volta
		else return 0;
	}
}

bool Mapa::VerificaJogadores(Jogador *jog) {

	int x = jog->getPosX();
	int y = jog->getPosY();

	
	for (int h = 0; h < jogs.size(); h++) {
		// jogador esta a direita.
		if (jogs[h]->getPosX() == x + 1 && jogs[h]->getPosY() == y) {
			return true;
		}
		// jogador esta a esquerda
		else if (jogs[h]->getPosX() == x - 1 && jogs[h]->getPosY() == y) {
			return true;
		}
		// jogador esta a baixo
		else if (jogs[h]->getPosX() == x && jogs[h]->getPosY() == y + 1) {
			return true;
		}
		// jogador esta em cima
		else if (jogs[h]->getPosX() == x && jogs[h]->getPosY() == y - 1) {
			return true;
		}
		// nao existe jogador em volta
		else return false;
	}

}

bool Mapa::VerificaJogador(Jogador *jog) {

	int x = jog->getPosX();
	int y = jog->getPosY();


	for (int h = 0; h < jogs.size(); h++) {
		if (jogs[h]->getPosX() == x && jogs[h]->getPosY() == y) {
			return true;
		}
		else return false;
	}
}

bool Mapa::VerificaJogadoresXY(int x, int y) {

	for (int h = 0; h < jogs.size(); h++) {
		if (jogs[h]->getPosX() == x && jogs[h]->getPosY() == y) {
			return true;
		}
		else return false;
	}
}

Jogador *Mapa::GuardaJogador(Jogador *jog) {

	Jogador *j = nullptr;

	int x = jog->getPosX();
	int y = jog->getPosY();

	for (int i = 0; i < jogs.size(); i++) {
			// jogador esta a direita.
			if (jogs[i]->getPosX() + 1 == x && jogs[i]->getPosY() == y) {
				return jogs[i];
			}
			// jogador esta a esquerda
			else if (jogs[i]->getPosX() - 1 == x && jogs[i]->getPosY() == y) {
				return jogs[i];
			}
			// jogador esta a baixo
			else if (jogs[i]->getPosX() == x && jogs[i]->getPosY() == y + 1) {
				return jogs[i];
			}
			// jogador esta em cima
			else if (jogs[i]->getPosX() == x && jogs[i]->getPosY() == y - 1) {
				return jogs[i];
			}
			// nao existe jogador em volta
			else return j;
	}
}

void Mapa::Combate(Jogador *jog) {

	Jogador *j = nullptr;

	// se exitir um jogador ao lado -> verificar a adjacencia

	if (GuardaJogador(jog) != nullptr) {
		
		j = GuardaJogador(jog);

		// se o jogador tiver uma pedra na mao
		if (jog->getPedra() == true) {

			j->setVida(j->getVida() - 2);

			// tirar a pedra do vetor de objeto;

			jog->retiraPedra();

			// adiciona pontos
			jog->setPontos(jog->getPontos() + 5);
		}

		// se nao tiver uma pedra ataca com as maos
		else {
			j->setVida(j->getVida() - 1);

			jog->setPontos(jog->getPontos() + 3);
		}

		if (j->getVida() <= 0) {
			jog->setPontos(jog->getPontos() + 10);
		}
	}

}

CelulaMapa &Mapa::getCelula(int x, int y) {
	return celula[x * colunas + y];
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

