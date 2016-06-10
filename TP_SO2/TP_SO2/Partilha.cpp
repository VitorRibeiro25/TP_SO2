#include "Partilha.h"


Partilha::Partilha() {
	parede = 0;
	jogador = -1;
	monstro = 0;
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
	if (monstro == 1) {
		return true;
	}
	else return false;
}

void Partilha::setParede() {
	parede = 1;
}

void Partilha::setJogador(int j) {
	jogador = j;
}

void Partilha::setMonstro(int m) {
	monstro = m;
}

void Partilha::setMutex(HANDLE m) {
	mutex = m;
}

int Partilha::PartilhaMapa(int x, int y) {

	return 0;
}

