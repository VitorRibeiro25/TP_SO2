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

void CelulaMapa::setParede(int p) {
	parede = p;
}

void CelulaMapa::setPorta(int p) {
	porta = p;
}