#include <iostream>
#include <string>
#include <tchar.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
using namespace std;

#define MAXCLIENTES 5

#define PIPENOME TEXT("\\\\.\\pipe\\teste")
#define PIPENOME2 TEXT("\\\\.\\pipe\\teste2")


HANDLE hPipe;
HANDLE hPipe2;
DWORD WINAPI Consola(LPVOID param);

struct resposta
{
	int ID_Cliente;
	bool JogadorLogado;
	bool jogoCriado;
	bool jogoIniciado;
	bool comandoErrado;
	TCHAR frase[256];
	TCHAR comandoErr[256];
	TCHAR nome[30];
};

struct resposta res;
static int ID_Cliente = 0;
static TCHAR Comando[256];

int _tmain(int argc, LPTSTR argv[]) {
	HANDLE hThread;
	DWORD n;


#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	//Menu();
	


	if (!WaitNamedPipe(PIPENOME, NMPWAIT_WAIT_FOREVER)) {
		_tprintf(TEXT("[INFO] Conectando... '%s'\n"), PIPENOME);
	}
	hPipe = CreateFile(PIPENOME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	hPipe2 = CreateFile(PIPENOME2, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hPipe == NULL || hPipe2 == NULL) {
		_tprintf(TEXT("[INFO] Nao e possivel conetar o servidor %s, tente mais tarde.\n"), PIPENOME);
		exit(-1);
	}

	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Consola, (LPVOID)hPipe2, 0, NULL);

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hPipe);
	Sleep(200);
	return 0;

}


DWORD WINAPI Consola(LPVOID param) {

	HANDLE hpipelocal = (HANDLE)param;
	DWORD n;
	BOOL ret;
	int i = 0, flag = 0, flag2 = 0;
	TCHAR buf[256];
	TCHAR Nome[25];
	TCHAR pass[25];
	TCHAR frase[256];

	//autenticação do utilizador - nome e pass
	
	_tprintf(TEXT("[Autenticação - coloque o nome]: "));
	fflush(stdin);
	_fgetts(Comando, 256, stdin);
	Comando[_tcslen(Comando) - 1] = '\0';
	wcscpy_s(Nome, Comando);
	res.ID_Cliente = ID_Cliente;
	WriteFile(hPipe, Comando, _tcslen(Comando) * sizeof(TCHAR), &n, NULL);

	_tprintf(TEXT("[Autenticação - coloque a passe]: "));
	fflush(stdin);
	_fgetts(Comando, 256, stdin);
	Comando[_tcslen(Comando) - 1] = '\0';
	wcscpy_s(pass, Comando);
	res.ID_Cliente = ID_Cliente;
	WriteFile(hPipe, Comando, _tcslen(Comando) * sizeof(TCHAR), &n, NULL);


	ReadFile(hpipelocal, buf, sizeof(buf), &n, NULL);
	buf[n / sizeof(TCHAR)] = '\0';

	_tprintf(TEXT("%s"), buf);
	_tprintf(TEXT("[Cliente %s] Seja bem vindo ao jogo\n"), Nome);

	while (1) {


		ret = ReadFile(hpipelocal, &res, sizeof(struct resposta), &n, NULL);
		if (!ret || !n) {
			_tprintf(TEXT("[Cliente -%s] O servidor desligou-se\n\n"), Nome);
			break;
		}

		if (ID_Cliente == 0) {
			ID_Cliente = res.ID_Cliente;
		}
		if (res.JogadorLogado == true) {

			if (res.jogoCriado == true && flag == 0) {
				_tprintf(TEXT("[Servidor] O jogo ja foi criado\n"));
				flag++;
			}
			if (res.jogoIniciado == true && flag2 == 0) {
				_tprintf(TEXT("[Servidor] O jogo ja foi iniciado\n"));
				flag2++;
			}

			if (res.comandoErrado == true){
				_tprintf(TEXT("%s"), res.comandoErr);
			}

			if (res.jogoCriado == true && res.jogoIniciado == true) {
				_tprintf(TEXT("%s"), res.frase);
			}

			_tprintf(TEXT("[%s - Comandos]: "), Nome);
			fflush(stdin);
			_fgetts(Comando, 256, stdin);
			Comando[_tcslen(Comando) - 1] = '\0';
			res.ID_Cliente = ID_Cliente;

			WriteFile(hPipe, Comando, _tcslen(Comando) * sizeof(TCHAR), &n, NULL);
		}

	}

}
