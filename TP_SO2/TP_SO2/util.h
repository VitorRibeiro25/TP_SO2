#pragma once
#ifndef UTIL
#define UTIL

#include <iostream>
#include <string>
#include <tchar.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>


using namespace std;

#ifdef UNICODE
#define tstring wstring
#define tcout  wcout
#define tcerr wcerr
#define tcin wcin
#else
#define tstring string 
#define tcout cout
#define tcin cin
#define tcerr cerr
#endif

class Sala {

};


class Jogador {
	int lentidao; // valor igual a 5
	int vida;  // valor igual a 10
			   // os joagaodres sao controlados por teclas e rato
public:

};

class Objeto {
	// talvez fazer derivadas da classe objeto para os objetos do enunciado, no entanto nao sei se faz muito sentido
};

class Monstro {

};

#endif