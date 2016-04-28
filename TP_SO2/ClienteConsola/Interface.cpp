#include "Interface.h"


void Menu() {
	int op;
	string nomePlayer;
	_tprintf(TEXT("\t\t-----------Menu-----------\n"));
	_tprintf(TEXT("\t\t-----------1 - Jogar-------\n"));
	_tprintf(TEXT("\t\t-----------2 - Sair------\n"));
	_tprintf(TEXT("\t\tResposta: "));
	tcin >> op;

	if (op == 1) {
		
		//chamar a funcao de registar e introduzir os dados
	}
	else
		if (op == 2)
			exit(0);
}

