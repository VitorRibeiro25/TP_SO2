#include "util.h"
#include "Engenho.h"
#include "Mapa.h"
#include "Jogador.h"


#define MAXCLIENTES 5

#define PIPENOME TEXT("\\\\.\\pipe\\teste")
#define PIPE2NOME TEXT("\\\\.\\pipe\\teste2")

#define REGISTRY_KEY TEXT("Software\\TPSO2\\");

#define MUTEX_NAME TEXT("O servidor está a correr?")

DWORD WINAPI ThreadLeituraEscritaInfo(LPVOID param);

HANDLE clientes[MAXCLIENTES], hEvent, hThread[MAXCLIENTES];

BOOL fim = FALSE;

Engenho *e;
Mapa *m;

struct resposta
{	
	int ID_Cliente;
	int vida;
	int pontuacao;
	bool JogadorLogado;
	bool jogoCriado;
	bool jogoIniciado;
	bool comandoErrado;
	TCHAR frase[256];
	TCHAR comandoErr[256];
	char nome[50];
	int mapa[20][20];
};

typedef struct {
	TCHAR nome[30];
	TCHAR pass[30];
	HANDLE pipe;
	HANDLE envia;
	HANDLE recebe;

}utilizador;

int numero = 0;
struct resposta res;
static int ID_Cliente = 0;
static TCHAR Comando[256];
utilizador utili[MAXCLIENTES];


void GameStatus() {
	_tprintf(TEXT("O jogo foi criado\n"));
}

int _tmain(int argc, LPTSTR argv[]) {
	HANDLE hpipe;
	OVERLAPPED ovl;
	e = new Engenho;

#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	ovl.hEvent = hEvent;

	_tprintf(TEXT("\t------Espera pelos clientes-------\n\n"));

	//Servidor esta a correr?
	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEX_NAME);

	if (hMutex == NULL)
	{
		// no duplicate instances found
		hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);
	}
	else
	{
		// a duplicate was found
		_tprintf(TEXT("Já existe uma instância do servidor a correr!\n"));
		return 0;
	}

	for (int i = 0; i < MAXCLIENTES && !fim; i++) {

		// duplex

		hpipe = CreateNamedPipe(PIPENOME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE
			| PIPE_READMODE_MESSAGE, MAXCLIENTES, 256, 256, 1000, NULL);

		// generic 

		clientes[i] = CreateNamedPipe(PIPE2NOME, PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE
			| PIPE_READMODE_MESSAGE, MAXCLIENTES, 256, 256, 1000, NULL);

		if (clientes[i] == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("Erro ao conectar o cliente\n"));
			exit(-1);
		}

		ConnectNamedPipe(clientes[i], &ovl);

		WaitForSingleObject(ovl.hEvent, INFINITE);

		// alterar para duas threads -> uma de leitura e uma de escrita

		hThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadLeituraEscritaInfo, (LPVOID)hpipe, 0, NULL);


	}

	WaitForMultipleObjects(MAXCLIENTES, hThread, FALSE, INFINITE);
	_tprintf(TEXT("O sistema vai ser desligado\n"));

	Sleep(1000);
	return 0;

}

void Autenticacao(LPVOID param) {

	DWORD n;
	HANDLE client = (HANDLE)param;
	TCHAR nome[256];
	TCHAR pass[256];
	TCHAR buf[256];
	BOOL ret, value;
	LPCTSTR pStr, pStr2;
	int p, existe;

	do {
		existe = 0;
		ret = ReadFile(client, nome, sizeof(nome), &n, NULL);
		nome[n / sizeof(TCHAR)] = '\0';
		pStr2 = nome;

		ret = ReadFile(client, pass, sizeof(pass), &n, NULL);
		pass[n / sizeof(TCHAR)] = '\0';

		for (p = 0; p < MAXCLIENTES; p++) {
			if (_tcscmp(nome, utili[p].nome) ==  0){
				existe = 1;
			}
		}

		if (existe == 0) {
			wcscpy_s(utili[numero].nome, nome);
			utili[numero].pipe = client;

			if (e->VerificaRegisto(nome) == true) {
				_tprintf(TEXT("[Servidor] O cliente %s ja está registado\n\n"), utili[numero].nome);

			}
			else {
				e->NovoRegisto(nome, pass);
				_tprintf(TEXT("[Servidor] O cliente tem o nome como: %s\n\n"), utili[numero].nome);
			}
			_tprintf(TEXT("[Servidor] Login com sucesso\n"));
			p = 1;
			WriteFile(clientes[numero], &p, sizeof(p), 0, NULL);
			_tcscpy_s(buf, 256, (TEXT("[Servidor] Voce esta ligado ao servidor\n\n")));
			WriteFile(clientes[numero], buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL);
			numero++;
		}
		else {
			_tprintf(TEXT("[Servidor] O cliente %d tentou logar-se com uma conta que já esta ativa! Movimento bloqueado\n"), numero);
			p = 0;
			WriteFile(clientes[numero], &p, sizeof(p), 0, NULL);
		}

	} while(existe!=0);

}

void FazerMapa(Jogador *jog) {
	//  jogador no meio 0 a 9 em cima, 0 a 9 a esquerda, 11 a 20 para baixo e a direita
	int num, nx = -11, ny = -11;
	int posx = 10 - jog->getPosX();//se for menor que 10 a conta é positiva
	int posy = 10 - jog->getPosY();

	if (posx > 0) {
		nx = -10 + posx;
	}
	if (posy > 0) {
		nx = -10 + posy;
	}
	for (int ij = -10; ij < 10; ij++) {
		for (int ji = -10; ji < 10; ji++) {//ver posição a posição pela função que criei
			if (ji < ny || ij < nx) {// se for negativo nunca se verifica isto ex: Posxy= 10-11
				res.mapa[ij][ji] = 9;    // ji=-10,  -10 < -1 + (-10) x cond. falsa
			}							//assim so se passar para fora dos limites
			else {
				num = m->Verificacelula((jog->getPosX() + ij), (jog->getPosY() + ji));
				res.mapa[ij + 10][ji + 10] = num;//o vetor começa no 0 qualquer valor do ij ou ji é sempre mais 10 para dar certo
			}
		}// a res vai ficar com o mapa
	}

	/*for (int ij = -10; ij < 10; ij++) {
	for (int ji = -10; ji < 10; ji++) {//ver posição a posição pela função que criei
	num = m->Verificacelula((jog->getPosX() + ij), (jog->getPosY()+ji));
	res.mapa[ij + 10][ji + 10] = num;//o vetor começa no 0 qualquer valor do ij ou ji é sempre mais 10 para dar certo
	}// a res vai ficar com o mapa
	}*/

	for (int ij = -10; ij < 10; ij++) {
		for (int ji = -10; ji < 10; ji++) {
			_tprintf(TEXT("%d "), res.mapa[ij + 10][ji + 10]);
		}
		_tprintf(TEXT("\n"));
	}
}


// falta colocar o handle para identificar o cliente em questao


DWORD WINAPI ThreadLeituraEscritaInfo(LPVOID param) {
	DWORD n;
	BOOL ret, value;
	int flag = 0, valorRetorno;
	TCHAR buf[256];
	TCHAR nome[256];
	TCHAR pass[256];
	TCHAR frase[256];
	TCHAR aux[256];
	Jogador *jog = new Jogador(numero);
	HANDLE client = (HANDLE)param;
	int ValidarCmd = -1;
	tstring sub1 = TEXT("");
	tstring sub2 = TEXT("");
	_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] O comando introduzido não está lista de comandos\n")));
	LPCTSTR pStr, pStr2;
	string Comando;
	string TipoComando;
	_tcscpy_s(nome, 256, (TEXT("")));
	srand(time(NULL));

	Autenticacao(param);

	do{
		sub1 = TEXT("");
		sub2 = TEXT("");
		ValidarCmd = -1;
		_tcscpy_s(buf, 256, (TEXT("[Servidor] Voce esta ligado ao servidor\n\n")));
		Comando = "";
		TipoComando = "";
		sub1.clear();
		sub2.clear();

		//enviaMensagem(jog);

		for (int i = 0; i < MAXCLIENTES; i++) {
			value = WriteFile(clientes[i], buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL);
			if (value == true ){
				_tprintf(TEXT("[Servidor] O cliente %d está logado e chama-se %s\n"), i, utili[i].nome);
			}
		}
		_tprintf(TEXT("\n\n"));

		for (int i = 0; i < MAXCLIENTES; i++) {
			if (jog->getId() == i) {
				res.JogadorLogado = true;
				WriteFile(clientes[i], &res, sizeof(struct resposta), 0, NULL);
			}
		}
		
		ret = ReadFile(client, buf, sizeof(buf), &n, NULL);
		if (!ret || !n) {
			for (int c = 0; c < MAXCLIENTES; c++) {
				if (client == utili[c].pipe) {
					_tprintf(TEXT("[Servidor] O cliente %d, o %s desligou-se\n\n"), c, utili[c].nome);
						for(int k = c; k < MAXCLIENTES; k++) {
							wcscpy_s(utili[k].nome, utili[k + 1].nome);
							utili[k].pipe = utili[k + 1].pipe;
						}
				}
			}
			break;
		}
		buf[n / sizeof(TCHAR)] = '\0';
		for (int y = 0; y < MAXCLIENTES; y++) {
			if (client == utili[y].pipe) {
				_tprintf(TEXT("[Servidor] O cliente %d, o %s mandou o comando: %s\n\n"), y, utili[y].nome, buf);
			}
		}

		pStr = buf;
		tstring str(pStr);
		sub1 = str.substr(str.find(TEXT(" ")) + 1); // ultima palavra
		sub2 = str.substr(0, str.length() - sub1.length() - 1);  //primeira palavra
		for (int i = 0; i < sub1.length(); i++)
			Comando += sub1.at(i);
		for (int i = 0; i < sub2.length(); i++)
			TipoComando += sub2.at(i);

		
		valorRetorno = e->ExecutaComando(TipoComando, Comando, jog);

		if (valorRetorno == 0) {
			_tprintf(TEXT("[Servidor] O comando que o cliente mandou não está na lista de comandos\n\n"));
			res.comandoErrado = true;
			_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] O comando introduzido não está lista de comandos\n")));
		}
		if (valorRetorno == 1) {
			res.jogoCriado = true;
			res.comandoErrado = false;
			for (int y = 0; y < MAXCLIENTES; y++) {
				if (client == utili[y].pipe) {
					_tprintf(TEXT("[Servidor] O cliente %s criou o jogo\n\n"), utili[y].nome);
				}
				m = new Mapa(50, 50);
				m->predefinido();
				
			}
		}
		if (valorRetorno == 2) {
			res.jogoIniciado = true;
			res.comandoErrado = false;
			for (int y = 0; y < MAXCLIENTES; y++) {
				if (client == utili[y].pipe) {
					_tprintf(TEXT("[Servidor] O cliente %s iniciou o jogo\n\n"), utili[y].nome);
					// fazer verificacao de paredes aqui - em falta
					int Posx = rand() % m->getLinhas();
					int Posy = rand() % m->getColunas();
					jog->setPosX(Posx);
					jog->setPosY(Posy);
					m->NovoJogador(jog);
				}
			}
		}
		if (valorRetorno == 3) {
			res.jogoCriado = true;
			res.jogoIniciado = true;
			res.comandoErrado = false;
			for (int y = 0; y < MAXCLIENTES; y++) {
				if (client == utili[y].pipe) {
					_tprintf(TEXT("[Servidor] O cliente %s juntou-se ao jogo\n\n"), utili[y].nome);
					// fazer verificacao de paredes aqui - em falta
					int x1 = rand() % m->getLinhas();
					int y1 = rand() % m->getColunas();
					jog->setPosX(x1);
					jog->setPosY(y1);
					m->NovoJogador(jog);
				}
			}
		}
		if (valorRetorno == 5) {
			res.comandoErrado = false;

			if (Comando == "direita") {

				if (m->VerificaAdjacencia(jog) == 3) {
					res.comandoErrado = true;
					_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] Encontra-se um jogador a sua direita\n")));
				}
				else jog->setPosY(jog->getPosY() + 1);

				if (m->VerificaParade(jog) == true) {
					res.comandoErrado = true;
					_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] Não é possivel mover para a direita\n")));
					jog->setPosY(jog->getPosY() - 1);
				}
				if (m->VerificaObjeto(jog) == true) {
					res.comandoErrado = true;
					_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] O cliente comeu um objeto\n")));
				}
			}
			if (Comando == "esquerda") {

				if (m->VerificaAdjacencia(jog) == 4) {
					res.comandoErrado = true;
					_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] Encontra-se um jogador a sua esquerda\n")));
				}
				else jog->setPosY(jog->getPosY() - 1);

				if (m->VerificaParade(jog) == true) {
					res.comandoErrado = true;
					_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] Não é possivel mover para a esquerda\n")));
					jog->setPosY(jog->getPosY() + 1);
				}

				if (m->VerificaObjeto(jog) == true) {
					res.comandoErrado = true;
					_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] O cliente comeu um objeto\n")));
				}
			}
			if (Comando == "cima") {

				if (m->VerificaAdjacencia(jog) == 2) {
					res.comandoErrado = true;
					_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] Encontra-se um jogador acima\n")));
				}
				else jog->setPosX(jog->getPosX() - 1);

				if (m->VerificaParade(jog) == true) {
					res.comandoErrado = true;
					_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] Não é possivel mover para cima\n")));
					jog->setPosX(jog->getPosX() + 1);
				}

				if (m->VerificaObjeto(jog) == true) {
					res.comandoErrado = true;
					_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] O cliente comeu um objeto\n")));
				}
			}
			if (Comando == "baixo") {

				if (m->VerificaAdjacencia(jog) == 1) {
					res.comandoErrado = true;
					_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] Encontra-se um jogador abaixo\n")));
				}
				else jog->setPosX(jog->getPosX() + 1);

				if (m->VerificaParade(jog) == true) {
					res.comandoErrado = true;
					_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] Não é possivel mover para baixo\n")));
					jog->setPosX(jog->getPosX() - 1);
				}

				if (m->VerificaObjeto(jog) == true) {
					res.comandoErrado = true;
					_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] O cliente comeu um objeto\n")));
				}
			}
		}

		if (valorRetorno == 6) {
			if (jog->verificaPedra() == true) {
				res.comandoErrado = true;
				jog->setPedra(true);
				_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] O Jogador colocou uma pedra na mao\n")));
			}
			else {
				res.comandoErrado = true;
				_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] O Jogador ainda nao tem pedras\n")));
			}
		}

		if (valorRetorno == 7) {
			if (m->VerificaJogadores(jog) == true) {
				m->Combate(jog);
				res.comandoErrado = true;
				_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] O Jogador atacou outro Jogador\n")));
			}
		}

		if (valorRetorno == 8) {
			res.comandoErrado = false;
			for (int y = 0; y < MAXCLIENTES; y++) {
				if (client == utili[y].pipe) {
					if (client == utili[y].pipe) {
						res.JogadorLogado = false;
						_tprintf(TEXT("[Servidor] O cliente %s fez logout do jogo\n\n"), utili[y].nome);
					}
				}
			}

		}
		if (valorRetorno == -1) {
			res.comandoErrado = false;
			_tprintf(TEXT("[Servidor] O jogo ainda nao foi criado ou iniciado\n"));
		}

		if (res.jogoCriado == true && res.jogoIniciado == true) {

			FazerMapa(jog);
			res.vida = jog->getVida();
			res.pontuacao = jog->getPontos();
			for (int i = 0; i < MAXCLIENTES; i++) {
				if (jog->getId() == i) {
					tstring aux = e->PosicaoJogador(jog);
					wcscpy_s(res.frase, aux.c_str());
					_tprintf(TEXT("%s"), res.frase);

				}
			}
		}

	} while (_tcsncmp(buf, TEXT("FIM"), 3));

	return 0;
}

