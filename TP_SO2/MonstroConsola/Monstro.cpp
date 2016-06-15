#include "Monstro.h"

int Monstro::id_m = 0;

Monstro::Monstro(TCHAR nome[TAM], int linhas, int colunas) {

	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("ObjetodePartilha"));

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
			GetLastError());
		return;
	}

	p = (Partilha*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, linhas*colunas*sizeof(Partilha));
	

	if (p == NULL)
	{
		exit(-1);
	}

	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEX_NAME);

	if (hMutex == NULL)
		exit(-1);

	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {
			p[i * colunas + j].setMutex(hMutex);
		}
	}

	wcscpy_s(this->nome, nome);

	this->linhas = linhas;
	this->colunas = colunas;

	ColocaMonstro();

}

void Monstro::ColocaMonstro() {

	srand(time(NULL));
	int x,y;

	do {
		x = 1 + (rand() % linhas);
		y = 1 + (rand() % colunas);

	} while (getPartilha(x, y).getParede());

	getPartilha(x, y).setMonstro(nome);
	id_m++;
	id = id_m;
	posX = x;
	posY = y;
}

void Monstro::mostraInfo() {
	cout << "Posicao do monstro X:" << posX << " Y:" << posY;
}

int Monstro::getPosX() {
	return posX;
}

int Monstro::getPosY() {
	return posY;
}

int Monstro::getVida() {
	return vida;
}

int Monstro::getId() {
	return id;
}

int Monstro::getLentidao() {
	return lentidao;
}

TCHAR Monstro::getNome() {
	return nome[TAM];
}

Partilha Monstro::getPartilha(int x, int y) {
	if (x < 0 || x > linhas || y < 0 || y > colunas) {
		return Partilha();
	}
	else return p[x * colunas + y];
}
	
void Monstro::setPosX(int x) {
	posX = x;
}

void Monstro::setPosY(int y) {
	posY = y;
}

void Monstro::setVida(int vid) {
	vida = vid;
}

void Monstro::setLentidao(int l) {
	lentidao = l;
}

void Monstro::setNome(TCHAR m[TAM]) {
	wcscpy_s(nome, m);
}


Monstro::~Monstro() {

}



int Monstro::movePosicao(Monstro *m) {


	return 0;
}

void Monstro::moveDist(int distancia) {

}
