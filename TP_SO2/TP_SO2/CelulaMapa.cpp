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

int CelulaMapa::getObjeto() {
	return objeto;
}


void CelulaMapa::setParede(int p) {
	parede = p;
}

void CelulaMapa::setChao(int p) {
	parede = p;
}

void CelulaMapa::setPorta(int p) {
	porta = p;
}

void CelulaMapa::setObjeto(int o) {
	objeto = o;
}