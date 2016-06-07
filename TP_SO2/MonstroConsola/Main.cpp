#include "Monstro.h"

struct monstro
{
	int vida;
	bool jogoIniciado;
	int linhas;
	int colunas;
	int mapa[20][20];
};

struct monstro monst;


void main() {

	Monstro *m;
	
	srand(time(NULL));

	int x = 1 + (rand() % monst.linhas);
	int y = 1 + (rand() % monst.colunas);


	m = new Monstro(x, y);

}