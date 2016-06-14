#include "Jogador.h"

int Jogador::id_inc = 0;


Jogador::Jogador(int id) {

	this->id = id;

	n_pedras = 0;

	setVida(10);
	setLentidao(5);
	setPedra(false);
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

int Jogador::getPontos() {
	return pontos;
}


int Jogador::getPosX() {
	return posX;
}

int Jogador::getPosY() {
	return posY;
}

int Jogador::getNpedras() {
	return n_pedras;
}

bool Jogador::getPedra() {
	return pedra;
}


bool Jogador::adicionaPedra(Objeto *ob) {

	int inc = 1;

	for (int i = 0; i < obj.size(); i++) {
		if (obj[i]->getNome() == "pedra") {
			inc++;
			n_pedras++;
		}
	}

	if (inc < 15) {
		return true;
	}
	else return false;

}

bool Jogador::verificaPedra() {
	
	bool aux = false;

	for (int i = 0; i < obj.size(); i++) {
		if (obj[i]->getNome() == "pedra") {
			aux  = true;
		}
	}

	return aux;
}

void Jogador::retiraPedra() {

	int inc = 1;

	for (int i = 0; i < obj.size(); i++) {
		if (obj[i]->getNome() == "pedra") {
			inc++;
			obj.erase(obj.begin() + i);
		}
	};

	if (inc == 0) {
		setPedra(false);
	}
}

void Jogador::ComeObjeto(Objeto *ob) {
	
	if (ob->getNome() == "vitamina") {
		this->setVida(this->getVida() + ob->getVida());
		obj.push_back(ob);
	}

	if (ob->getNome() == "orangebull") {
		this->setVida(this->getVida() + ob->getVida());
		obj.push_back(ob);
	}

	if (ob->getNome() == "pedra") {
		if (adicionaPedra(ob) == true) {
			// pedra adicionada ao vetor, primeiro verifica se tem o limite maximo que é 15.
			obj.push_back(ob);
		}
		
	}

	if (ob->getNome() == "rebucado") {
		this->setLentidao(this->getLentidao() + ob->getLentidao());
		obj.push_back(ob);
	}

}

string Jogador::UltimoObjetos() {
	
	string aux;

	for (int i = 0; i < obj.size(); i++) {
		aux = obj[i]->getNome();
	}

	return aux;

	//return obj.back()->getNome();
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

void Jogador::setPedra(bool p) {
	pedra = p;
}

void Jogador::setPontos(int pon) {
	pontos = pon;
}