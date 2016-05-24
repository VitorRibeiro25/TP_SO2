#include "Jogador.h"

int Jogador::id_inc = 0;


Jogador::Jogador(int id) {

	this->id = id;

	setVida(10);
	setLentidao(5);
}


Jogador::~Jogador() {

}

int Jogador::getId() {
	return id;
}

int Jogador::getVida() {
	return vida;
}

int Jogador::getLentidao() {
	return lentidao;
}

string Jogador::getNome() {
	return nome;
}

int Jogador::getPosX() {
	return posX;
}

int Jogador::getPosY() {
	return posY;
}

HANDLE Jogador::getRecebe() {
	return hrecebe;
}

HANDLE Jogador::getEnvia() {
	return henvia;
}

void Jogador::setNome(string na) {
	nome = na;
}

void Jogador::setVida(int v) {
	this->vida = v;
}

void Jogador::setLentidao(int l) {
	this->lentidao = l;
}

void Jogador::setPosX(int x) {
	posX = x;
}

void Jogador::setPosY(int y) {
	posY = y;
}