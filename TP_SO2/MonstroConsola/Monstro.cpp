#include "Monstro.h"

int Monstro::id_m = 0;


Monstro::Monstro(int x, int y) {
	posX = x;
	posY = y;

	id_m++;
	id = id_m;

	int i = 1 + (rand() % 2);

	// distraido
	if (i == 1) {
		setNome("distraido");
		setLentidao(3);
		setVida(15);

	}
	// bully
	if (i == 2) {
		setNome("bully");
		setLentidao(7);
		setVida(20);
	}

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

string Monstro::getNome() {
	return nome;
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

void Monstro::setNome(string n) {
	nome = n;
}


Monstro::~Monstro() {

}

int Monstro::movePosicao(Monstro *m) {


	return 0;
}

void Monstro::combate(Monstro *m) {

}