#include "CelulaMapa.h"


CelulaMapa::CelulaMapa() {

	obj = nullptr;
	jog = nullptr;
}


int CelulaMapa::getParede() {
	return parede;
}

int CelulaMapa::getPorta() {
	return porta;
}

void CelulaMapa::CriaObjeto(int x, int y) {
	
	int aux = rand() % 15;

	if (aux == 0){
		obj = new Objeto(x,y);
	}

}

void CelulaMapa::ColocaJogador(int x, int y, int id) {
	//jog = new Jogador(x,y,id);

}

void CelulaMapa::setParede(int p) {
	parede = p;
}

void CelulaMapa::setPorta(int p) {
	porta = p;
}