#include "Partilha.h"


Partilha::Partilha() {
	parede = 0;
	jogador = -1;
	wcscpy_s(monstro,TEXT(" "));
}

bool Partilha::getParede() {
	if (parede == 1) {
		return true;
	}
	else return false;
}

bool Partilha::asJogador() {
	if (jogador == 1)
		return true;
	else return false;
}

bool Partilha::asMonstro() {
	TCHAR b[25] = TEXT(" ");
	if (_tcscmp(monstro, b) == 0)
		return false;
	else return true;
}

void Partilha::setParede() {
	parede = 1;
}

void Partilha::setJogador(int j) {
	jogador = j;
}

void Partilha::setMonstro(TCHAR m[TAM]) {
	wcscpy_s(monstro, m);
}

void Partilha::setMutex(HANDLE m) {
	mutex = m;
}

int Partilha::PartilhaMapa(int x, int y) {

	return 0;
}