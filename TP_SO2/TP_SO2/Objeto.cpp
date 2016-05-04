#include "Objeto.h"

Objeto::Objeto(int x, int y) {
	
	int i;

	i = 1 + (rand() % 4);

	posX = x;
	posY = y;

	// vitaminas
	if (i == 1) {
		j->setVida(j->getVida() + 1);
	}

	// garrafas orangebull
	if (i == 2) {

	}

	// pedra
	if (i == 3) {

	}

	// rebuçado de cafeína
	if (i == 4) {

	}
	
}

Objeto::~Objeto() {

}

string Objeto::getNome() {
	return nome;
}

int Objeto::getPosX() {
	return posX;
}

int Objeto::getPosY() {
	return posY;
}

int Objeto::getVida() {
	return vida;
}

int Objeto::getLentidao() {
	return lentidao;
}

void Objeto::setPosX(int x) {
	posX = x;
}

void Objeto::setPosY(int y) {
	posY = y;
}

void Objeto::setNome(string n) {
	nome = n;
}

void Objeto::setVida(int v) {
	vida = v;
}

void Objeto::setLentidao(int l) {
	lentidao = l;
}