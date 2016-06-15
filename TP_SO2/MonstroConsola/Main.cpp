#include "Monstro.h"

#define TAM 256



int _tmain(int argc, LPTSTR argv[]) {

	Monstro *m;
	cout << argc << endl;
	TCHAR nome[TAM];
	cout << argv[1] << endl;
	wcscpy_s(nome, argv[0]);
	int linhas = _ttoi(argv[1]);
	int colunas = _ttoi(argv[2]);
	int n_casas = _ttoi(argv[3]);

	cout << nome << endl;
	m = new Monstro(nome, linhas, colunas);

	//m->mostraInfo();

	while (true) {
		if (_tcscmp(nome, TEXT("Distraido")) == 0) {
			m->moveDist(n_casas);
		}
		else if (_tcscmp(nome, TEXT("Bully")) == 0) {
			m->moveDist(n_casas);
		}

	}


}