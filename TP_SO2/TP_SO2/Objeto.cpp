#include "Objeto.h"

Objeto::Objeto(int x, int y) {
	
	int i;
	posX = x;
	posY = y;

	i = 1 + (rand() % 4);

	// vitaminas
	if (i == 1) {
		setNome("vitamina");
		setVida(1);
		setLentidao(0);
	}

	// garrafas orangebull
	if (i == 2) {
		setNome("orangebull");
		setVida(3);
		setLentidao(0);
	}

	// pedra
	if (i == 3) {
		setNome("pedra");
		setLentidao(0);
		setVida(0);
	}

	// rebuçado de cafeína
	
	if (i == 4) {
		setNome("rebucado");
		setLentidao(-2);
		setVida(0);
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