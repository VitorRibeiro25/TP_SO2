#include "util.h"
#include "Engenho.h"
#include "Mapa.h"
#include "Jogador.h"


#define MAXCLIENTES 5

#define PIPENOME TEXT("\\\\.\\pipe\\teste")
#define PIPE2NOME TEXT("\\\\.\\pipe\\teste2")

#define REGISTRY_KEY TEXT("Software\\TPSO2\\");

DWORD WINAPI ThreadLeituraEscritaInfo(LPVOID param);

HANDLE clientes[MAXCLIENTES], hEvent;	
BOOL fim = FALSE;

Engenho *e;
Mapa *m;

struct resposta
{	
	int ID_Cliente;
	bool JogadorLogado;
	bool jogoCriado;
	bool jogoIniciado;
	int EsperaPlayers;
	TCHAR frase[256];
	char nome[50];
};

typedef struct {
	TCHAR nome[30];
	TCHAR pass[30];
	HANDLE pipe;
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

	HANDLE hThread = NULL;
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

		// Incrementar valor dos players ativos


		hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadLeituraEscritaInfo, (LPVOID)hpipe, 0, NULL);



	}

	WaitForSingleObject(hThread, INFINITE);
	_tprintf(TEXT("O sistema vai ser desligado\n"));

	Sleep(1000);
	return 0;

}


DWORD WINAPI ThreadLeituraEscritaInfo(LPVOID param) {
	DWORD n;
	BOOL ret, value;
	int flag = 0, valorRetorno;
	TCHAR buf[256];
	TCHAR nome[256];
	TCHAR pass[256];
	TCHAR frase[256];
	Jogador *jog = nullptr;
	HANDLE client = (HANDLE)param;
	int ValidarCmd = -1;
	tstring sub1 = TEXT("");
	tstring sub2 = TEXT("");
	LPCTSTR pStr, pStr2;
	string Comando;
	string TipoComando;
	_tcscpy_s(nome, 256, (TEXT("")));
	res.jogoCriado = false;
	res.jogoIniciado = false;


	ret = ReadFile(client, nome, sizeof(nome), &n, NULL);
	nome[n / sizeof(TCHAR)] = '\0';
	pStr2 = nome;

	ret = ReadFile(client, pass, sizeof(pass), &n, NULL);
	pass[n / sizeof(TCHAR)] = '\0';


	wcscpy_s(utili[numero].nome, nome);
	utili[numero].pipe = client;	

	if (e->VerificaRegisto(nome) == true) {
		_tprintf(TEXT("[Servidor] O cliente %s ja está registado\n\n"), utili[numero].nome);

	}
	else {
		e->NovoRegisto(nome, pass);
		_tprintf(TEXT("[Servidor] O cliente tem o nome como: %s\n\n"), utili[numero].nome);
	}

	numero++;

	do{
		sub1 = TEXT("");
		sub2 = TEXT("");
		ValidarCmd = -1;
		_tcscpy_s(buf, 256, (TEXT("[Servidor] Voce esta ligado ao servidor\n\n")));
		Comando = "";
		TipoComando = "";
		sub1.clear();
		sub2.clear();

		for (int i = 0; i < MAXCLIENTES; i++) {
			value = WriteFile(clientes[i], buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL);
			if (value == true ){
				_tprintf(TEXT("[Servidor] O cliente %d está logado e chama-se %s\n"), i, utili[i].nome);
			}
		}
		_tprintf(TEXT("\n\n"));

		for (int i = 0; i < MAXCLIENTES; i++) {
			res.JogadorLogado = true;
			WriteFile(clientes[i], &res, sizeof(struct resposta), 0, NULL);
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
		}
		if (valorRetorno == 1) {
			res.jogoCriado = true;
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
			for (int y = 0; y < MAXCLIENTES; y++) {
				if (client == utili[y].pipe) {
					_tprintf(TEXT("[Servidor] O cliente %s iniciou o jogo\n\n"), utili[y].nome);
					int x = rand() % m->getLinhas();
					int y = rand() % m->getColunas();
					jog = new Jogador(x, y);
					m->NovoJogador(x, y);
				}
			}
		}
		if (valorRetorno == 3) {
			for (int y = 0; y < MAXCLIENTES; y++) {
				if (client == utili[y].pipe) {
					_tprintf(TEXT("[Servidor] O cliente %s juntou-se ao jogo\n\n"), utili[y].nome);
					int x1 = rand() % m->getLinhas();
					int y1 = rand() % m->getColunas();
					m->NovoJogador(x1, y1);
				}
			}
		}
		if (valorRetorno == 5) {
			if (Comando == "direita") {
				jog->setPosY(jog->getPosY() + 1);
			}
			if (Comando == "esquerda") {
				jog->setPosY(jog->getPosY() - 1);
			}
			if (Comando == "cima") {
				jog->setPosX(jog->getPosX() + 1);
			}
			if (Comando == "baixo") {
				jog->setPosX(jog->getPosX() - 1);
			}
		}
		if (valorRetorno == 7) {
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
			_tprintf(TEXT("[Servidor] O jogo ainda nao foi criado ou iniciado\n\n"));
		}

		if (res.jogoCriado == true && res.jogoIniciado == true) {

			tstring aux = e->PosicaoJogador(jog);
			wcscpy_s(res.frase, aux.c_str());
		}

		for (int i = 0; i < MAXCLIENTES; i++) {
			WriteFile(clientes[i], &res, sizeof(struct resposta), 0, NULL);
		}

	} while (_tcsncmp(buf, TEXT("FIM"), 3));

	return 0;
}

