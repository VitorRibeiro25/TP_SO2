#include "util.h"

#define MAXCLIENTES 5

#define PIPENOME TEXT("\\\\.\\pipe\\teste")
#define PIPE2NOME TEXT("\\\\.\\pipe\\teste2")
DWORD WINAPI ThreadLeituraEscritaInfo(LPVOID param);

HANDLE clientes[MAXCLIENTES], hEvent;	
BOOL fim = FALSE;

struct resposta
{
	int ID_Cliente;
	bool JogoCriado;
	bool JogoIniciado;
	int EsperaPlayers;
};

struct resposta res;
static int ID_Cliente = 0;
static TCHAR Comando[256];


void GameStatus() {
	_tprintf(TEXT("O jogo foi criado\n"));
}

int _tmain(int argc, LPTSTR argv[]) {

	HANDLE hThread = NULL;
	HANDLE hpipe;
	OVERLAPPED ovl;

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
	int i;
	TCHAR buf[256];
	int aux;
	HANDLE client = (HANDLE)param;
	int ValidarCmd = -1;
	tstring sub1;
	tstring sub2;
	LPCTSTR pStr;
	string Comando = "";
	string TipoComando = "";
	
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
			if (value == true) {
				_tprintf(TEXT("[Servidor] O cliente ficou agora logado\n\n"));
			}
		}


		for (int i = 0; i < MAXCLIENTES; i++) {
			res.JogoCriado = true;
			res.JogoIniciado = true;
			WriteFile(clientes[i], &res, sizeof(struct resposta), 0, NULL);
		}
		
		ret = ReadFile(client, buf, sizeof(buf), &n, NULL);
		if (!ret || !n)
			break;
		buf[n / sizeof(TCHAR)] = '\0';
		_tprintf(TEXT("[Servidor] O cliente mandou -%s\n\n"), buf);

		pStr = buf;
		tstring str(pStr);
		sub1 = str.substr(str.find(TEXT(" ")) + 1);// ult palavra
		sub2 = str.substr(0, str.length() - sub1.length() - 1);  //prim palavra
		for (int i = 0; i < sub1.length(); i++)
			Comando += sub1.at(i);
		for (int i = 0; i < sub2.length(); i++)
			TipoComando += sub2.at(i);

		GameStatus();

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

		}*/
	} while (_tcsncmp(buf, TEXT("FIM"), 3));

}