#include "util.h"
#include "Engenho.h"
#include "Mapa.h"


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
	bool JogoLogado;
	int EsperaPlayers;
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
	int i, flag = 0, valorRetorno;
	TCHAR buf[256];
	TCHAR nome[256];
	TCHAR pass[256];
	int aux;
	HANDLE client = (HANDLE)param;
	int ValidarCmd = -1;
	tstring sub1 = TEXT("");
	tstring sub2 = TEXT("");
	LPCTSTR pStr, pStr2, pStr3;
	LPCWSTR NOME;
	string Comando;
	string TipoComando;
	_tcscpy_s(nome, 256, (TEXT("")));


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
			res.JogoLogado = true;
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
		sub1 = str.substr(str.find(TEXT(" ")) + 1);// ult palavra
		sub2 = str.substr(0, str.length() - sub1.length() - 1);  //prim palavra
		for (int i = 0; i < sub1.length(); i++)
			Comando += sub1.at(i);
		for (int i = 0; i < sub2.length(); i++)
			TipoComando += sub2.at(i);

		
		valorRetorno = e->ExecutaComando(TipoComando, Comando);

		if (valorRetorno == 0) {
			_tprintf(TEXT("[Servidor] O comando que o cliente mandou não está na lista de comandos\n\n"));
		}
		if (valorRetorno == 1 && e->getJogoCriado() == false) {
			for (int y = 0; y < MAXCLIENTES; y++) {
				if (client == utili[y].pipe) {
					_tprintf(TEXT("[Servidor] O cliente %s criou o jogo\n\n"), utili[y].nome);
				}
			}
		}
		if (valorRetorno == 2 && e->getJogoCriado() == true && e->getJogoIniciado() == false) {
			for (int y = 0; y < MAXCLIENTES; y++) {
				if (client == utili[y].pipe) {
					_tprintf(TEXT("[Servidor] O cliente %s iniciou o jogo\n\n"), utili[y].nome);
				}
			}
		}

		/*
		res.JogoCriado = e.getJogoCriadoEstado();
		res.JogoIniciado = e.getJogoIniciadoEstado();
		res.ID_Cliente = t.getNJog();

		if (e.getJogoCriadoEstado() == true && e.getJogoIniciadoEstado() == false) {
			wcout << "Enviou mensagem para ultimo cliente" << endl;
			WriteFile(client, &res, sizeof(struct resposta), &n, NULL);
		}

		if (e.getJogoCriadoEstado() == true && e.getJogoIniciadoEstado() == true) {
			UpdateMapa();
			for (i = 0; i <PlayersActive; i++) {
				WriteFile(hPipeAct[i], &res, sizeof(struct resposta), &n, NULL);
			}
			_tprintf(TEXT("Foram avisados os clientes de uma actualização.\n"), n);
		}
		else {
			system("cls");
			_tprintf(TEXT("Espera dos restantes players.\n"));

		}
		
		for (int i = 0; i < MAXCLIENTES; i++) {
			if (!ret)
				_tprintf(TEXT("[Servidor] O cliente desligou-se\n\n"));
				
		}*/

	} while (_tcsncmp(buf, TEXT("FIM"), 3));

}

