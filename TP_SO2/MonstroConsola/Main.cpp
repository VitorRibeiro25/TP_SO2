#include "Monstro.h"

#define TAM 256



int _tmain(int argc, LPTSTR argv[]) {

	Monstro *m;

	TCHAR nome[TAM];
	wcscpy_s(nome, argv[1]);
	int linhas = _ttoi(argv[2]);
	int colunas = _ttoi(argv[3]);
	int n_casas = _ttoi(argv[4]);

	m = new Monstro(nome, linhas, colunas);

	m->mostraInfo();

	while (true) {
		if (_tcscmp(nome, TEXT("Distraido")) == 0) {
			m->moveDist(n_casas);
		}
		else if (_tcscmp(nome, TEXT("Bully")) == 0) {
			m->moveDist(n_casas);
		}

	}


}