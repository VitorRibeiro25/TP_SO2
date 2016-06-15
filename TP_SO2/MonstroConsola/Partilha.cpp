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
	if (_tcscmp(monstro, TEXT(" ")) == 0)
		return false;
	else return true;
}

void Partilha::setParede() {
	parede = 1;
}

void Partilha::setJogador(int j) {
	jogador = j;
}

void Partilha::setMonstro(TCHAR *m) {
	cout << m << endl;
	wcscpy_s(monstro, m);
	cout << monstro << endl;
}

void Partilha::setMutex(HANDLE m) {
	mutex = m;
}

int Partilha::PartilhaMapa(int x, int y) {

	return 0;
}