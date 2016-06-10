#include "Monstro.h"

#define TAM 256



int _tmain(int argc, LPTSTR argv[]) {

	
	Monstro *m;

	int linhas = _ttoi(argv[1]);
	int colunas = _ttoi(argv[2]);
	int n_casas = _ttoi(argv[3]);

	m = new Monstro(linhas, colunas);

	m->mostrainfo();

	while (true)
	{
		
	}



}