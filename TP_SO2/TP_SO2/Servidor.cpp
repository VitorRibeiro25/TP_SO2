#include "util.h"

#define MAXCLIENTES 5

#define PIPENOME TEXT("\\\\.\\pipe\\teste")
#define PIPE2NOME TEXT("\\\\.\\pipe\\teste2")
DWORD WINAPI ThreadLeituraEscritaInfo(LPVOID param);

HANDLE clientes[MAXCLIENTES], hEvent;	
BOOL fim = FALSE;



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

	_tprintf(TEXT("\t------Espera pelos clientes-------"));

	for (int i = 0; i < MAXCLIENTES && !fim; i++) {

		// duplex

		hpipe = CreateNamedPipe(PIPENOME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE
			| PIPE_READMODE_MESSAGE, MAXCLIENTES, 256, 256,
			1000, NULL);

		// generic 

		clientes[i] = CreateNamedPipe(PIPE2NOME, PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE
			| PIPE_READMODE_MESSAGE, MAXCLIENTES, 256, 256,
			1000, NULL);

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

	return 0;

}