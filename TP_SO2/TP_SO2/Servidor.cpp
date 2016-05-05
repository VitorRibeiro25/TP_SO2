#include "util.h"


#define MAXCLIENTES 5

#define PIPENOME TEXT("\\\\.\\pipe\\teste")
#define PIPE2NOME TEXT("\\\\.\\pipe\\teste2")

#define REGISTRY_KEY TEXT("Software\\TPSO2\\");

DWORD WINAPI ThreadLeituraEscritaInfo(LPVOID param);

HANDLE clientes[MAXCLIENTES], hEvent;	
BOOL fim = FALSE;

struct resposta
{	
	int ID_Cliente;
	bool JogoCriado;
	bool JogoIniciado;
	int EsperaPlayers;
	char nome[50];
};

typedef struct {
	TCHAR nome[35];
	TCHAR pass[35];
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

bool VerificaRegisto(LPCWSTR nome, LPCWSTR pass) {

	HKEY chave;
	DWORD queAconteceu, versao, tamanho;
	TCHAR autor[200];
	TCHAR keyName[200] = REGISTRY_KEY;
	wcscat_s(keyName, nome);
	
	//Criar/abrir uma chave em HKEY_CURRENT_USER\Software\MinhaAplicacao
	if (RegCreateKeyEx(HKEY_CURRENT_USER, keyName, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &chave, &queAconteceu) != ERROR_SUCCESS) {
		tcout << TEXT("Erro ao criar/abrir chave (%d)\n"), GetLastError();
		return false;
	}
	else {
		if (queAconteceu == REG_CREATED_NEW_KEY) {
			// fazer novo registo de um novo cliente, depois da chave ja estar a aberta

			RegSetValueEx(chave, TEXT("Nome"), 0, REG_SZ, (LPBYTE)nome, _tcslen(nome)*sizeof(TCHAR));

			RegSetValueEx(chave, TEXT("Pass"), 0, REG_SZ, (LPBYTE)pass, _tcslen(pass)*sizeof(TCHAR));
		}
		else if (queAconteceu == REG_OPENED_EXISTING_KEY) {
			//Se a chave foi aberta, ler os valores lá guardadoss

			RegQueryValueEx(chave, TEXT("Nome"), NULL, NULL, (LPBYTE)nome, &tamanho);

			RegQueryValueEx(chave, TEXT("Nome"), NULL, NULL, (LPBYTE)pass, &tamanho);

			return true;
		}


	}

}




DWORD WINAPI ThreadLeituraEscritaInfo(LPVOID param) {
	DWORD n;
	BOOL ret, value;
	int i, flag = 0;
	TCHAR buf[256];
	TCHAR nome[256];
	TCHAR pass[256];
	int aux;
	HANDLE client = (HANDLE)param;
	int ValidarCmd = -1;
	tstring sub1;
	tstring sub2;
	LPCTSTR pStr;
	string Comando = "";
	string TipoComando = "";
	_tcscpy_s(nome, 256, (TEXT("")));


	ret = ReadFile(client, nome, sizeof(nome), &n, NULL);
	nome[n / sizeof(TCHAR)] = '\0';
	ret = ReadFile(client, pass, sizeof(pass), &n, NULL);
	pass[n / sizeof(TCHAR)] = '\0';
	

	if (VerificaRegisto(nome, pass) == true) {
		_tprintf(TEXT("[Servidor] O cliente ja está registado\n\n"));
	}

	wcscpy_s(utili[numero].nome, nome);
	utili[numero].pipe = client;
	_tprintf(TEXT("[Servidor] O cliente tem o nome como: %s\n\n"), utili[numero].nome);
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
				_tprintf(TEXT("[Servidor] O cliente %d ficou agora logado e chama-se %s\n"), i, utili[i].nome);
			}
		}
		_tprintf(TEXT("\n\n"));

		for (int i = 0; i < MAXCLIENTES; i++) {
			res.JogoCriado = true;
			res.JogoIniciado = true;
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

