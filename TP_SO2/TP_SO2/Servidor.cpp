#include "util.h"
#include "Engenho.h"
#include "Mapa.h"
#include "Jogador.h"
#include "Partilha.h"

#define SIZE 256

#define MAXCLIENTES 5

#define PIPENOME TEXT("\\\\.\\pipe\\teste")
#define PIPE2NOME TEXT("\\\\.\\pipe\\teste2")

#define REGISTRY_KEY TEXT("Software\\TPSO2\\");

#define CAMINHO TEXT("C:\\Users\\Vitor Ribeiro\\Documents\\GitHub\\TP_SO2\\TP_SO2\\Debug\\MonstroConsola.exe")

#define MUTEX_NAME TEXT("O servidor está a correr?")
#define MUTEX_NAME2 TEXT("Mutex a funcionar?")


DWORD WINAPI ThreadLeituraEscritaInfo(LPVOID param);

HANDLE clientes[MAXCLIENTES], hEvent, hThread[MAXCLIENTES];

BOOL fim = FALSE;

Engenho *e;
Mapa *m;
Partilha *p;


struct resposta
{	
	int ID_Cliente;
	int vida;
	int pontuacao;
	int pedras;
	int lin;
	int col;
	BOOL JogadorLogado;
	BOOL jogoCriado;
	BOOL jogoIniciado;
	BOOL comandoErrado;
	TCHAR frase[256];
	TCHAR comandoErr[256];
	char nome[50];
	int mapa[9][9];
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
HANDLE hMapFile;
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
			p = 1;
			WriteFile(clientes[numero], &p, sizeof(p), 0, NULL);

			_tcscpy_s(buf, 256, (TEXT("[Servidor] Voce não esta ligado ao servidor\n")));
			WriteFile(clientes[numero], buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL);
		}

	} while(existe!=0);

}

bool VerificaMonstro(int x, int y) {
	for (int i = 0; i < m->getLinhas(); i++) {
		for (int j = 0; j < m->getColunas(); j++) {
			if (i == 1 && j == 1) {
				if (p[i*m->getColunas() + j].asMonstro() == true) {
					return true;
				}
				else return false;
			}
			tcout << p[i*m->getColunas() + j].asMonstro();
		}
	}
}


int Verificacelula(int x, int y) {

	bool parede, monstro;
	int objeto, jogador;

	if (x < 0 || y < 0 || x > m->getLinhas() || y > m->getColunas()) {
		return -1; //fora do mapa
	}
	else {

		parede = m->VerificaParede(x, y);
		objeto = m->verifaObjetoNome(x, y);
		jogador = m->VerificaJogadoresXY(x, y);
		monstro = VerificaMonstro(x, y);

		if (parede == true) {
			return 1; // parede é 1 
		}
		else if (objeto == 2) {
			return 2; //objeto vitamina
		}
		else if (objeto == 3) {
			return 3; // objeto orangebull
		}
		else if (objeto == 4) {
			return 4;  // objeto pedra
		}
		else if (objeto == 5 ) {
			return 5;  // objeto rebucado
		}
		else if (jogador == 6) {
			return 6; //jogador 1 é 6
		}
		else if (jogador == 7) {
			return 7; //jogador 2 é 7
		}
		else if (jogador == 8) {
			return 8; //jogador 3 é 8
		}
		else if (jogador == 9) {
			return 9; //jogador 4 é 9
		}
		else if (jogador == 10) {
			return 10; //jogador 5 é 10
		}
		else if (monstro == true) {
			return 11; 
		}
		else {
			return 0; //É so chão
		}
	}


}

void FazerMapa(Jogador *jog) {

	//  jogador no meio 0 a 9 em cima, 0 a 9 a esquerda, 11 a 20 para baixo e a direita
	int num, nx = -11, ny = -11;
	int posx = 10 - jog->getPosX(); //se for menor que 10 a conta é positiva
	int posy = 10 - jog->getPosY();
	int id_u = jog->getId();

	if (posx > 0) {
		nx = -10 + posx;
	}
	if (posy >0) {
		ny = -10 + posy;
	}
	for (int ij = -4; ij <= 4; ij++) {
		for (int ji = -4; ji <= 4; ji++) {//ver posição a posição pela função que criei
				num = Verificacelula((jog->getPosX() + ij), (jog->getPosY() + ji));
				res.mapa[ij + 4][ji + 4] = num;//o vetor começa no 0 qualquer valor do ij ou ji é sempre mais 10 para dar certo
			
		}// a res vai ficar com o mapa
	}

	/*for (int ij = -10; ij < 10; ij++) {
	for (int ji = -10; ji < 10; ji++) {//ver posição a posição pela função que criei
	num = m->Verificacelula((jog->getPosX() + ij), (jog->getPosY()+ji));
	res.mapa[ij + 10][ji + 10] = num;//o vetor começa no 0 qualquer valor do ij ou ji é sempre mais 10 para dar certo
	}// a res vai ficar com o mapa
	}*/

	for (int ij = -4; ij <=4; ij++) {
		for (int ji = -4; ji <= 4; ji++) {
			_tprintf(TEXT("%d "), res.mapa[ij + 4][ji + 4]);
		}
		_tprintf(TEXT("\n"));
	}
}

void PartilhaMonstro() {

	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, m->getLinhas()*m->getColunas()*sizeof(Partilha), TEXT("ObjetodePartilha"));

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
			GetLastError());
		return;
	}

	p = (Partilha*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, m->getLinhas()*m->getColunas()*sizeof(Partilha));


	if (p == NULL)
	{
		return;
	}

	Partilha * tempP = new Partilha[m->getLinhas()*m->getColunas()];

	CopyMemory(p, tempP, m->getLinhas()*m->getColunas()*sizeof(Partilha));

	delete tempP;

	HANDLE tempMutex = CreateMutex(NULL, FALSE, MUTEX_NAME2);

	if (tempMutex == NULL)
		exit(-1);

	for (int i = 0; i < m->getLinhas(); i++)
	{
		for (int j = 0; j < m->getColunas(); j++)
		{
			p[i * m->getColunas() + j].setMutex(tempMutex);
			if (m->getCelula(i, j).getParede() == 1)
				p[i* m->getColunas() + j].setParede();
		}
	}
}

void PartilhaJogador(Jogador *jog) {

	p[jog->getPosX() *m->getColunas() + jog->getPosY()].setJogador(numero);
	
}



void MandaMonstro(TCHAR tipo[25])
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	TCHAR args[SIZE];
	int n_casas = 2;

	tstringstream aux;
	aux << tipo << TEXT(" ") << m->getLinhas() << TEXT(" ") << m->getColunas() << TEXT(" ") << n_casas;

	tcout << aux.str() << endl;

	wcscpy_s(args, (aux.str()).c_str());
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	//ZeroMemory(&pi, sizeof(pi));

	tcout << args << endl;

	CreateProcess(CAMINHO, args, NULL, NULL, 0, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
}


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
		_tcscpy_s(buf, 256, (TEXT("[Servidor] Voce esta ligado")));
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
			if (Comando == "pre") {
				for (int y = 0; y < MAXCLIENTES; y++) {
					if (client == utili[y].pipe) {
						_tprintf(TEXT("[Servidor] O cliente %s criou o jogo\n\n"), utili[y].nome);
					}
					m = new Mapa(50, 50);
					m->predefinido();
					PartilhaMonstro();

				}
				MandaMonstro(TEXT("Bully"));
			}
			if (Comando == "ran") {
				for (int y = 0; y < MAXCLIENTES; y++) {
					if (client == utili[y].pipe) {
						_tprintf(TEXT("[Servidor] O cliente %s criou o jogo\n\n"), utili[y].nome);
					}
					m = new Mapa(res.lin, res.col);
					m->predefinido();
					PartilhaMonstro();

				}
			}
		}
		if (valorRetorno == 2) {
			res.jogoIniciado = true;
			res.comandoErrado = false;
			for (int y = 0; y < MAXCLIENTES; y++) {
				if (client == utili[y].pipe) {
					_tprintf(TEXT("[Servidor] O cliente %s iniciou o jogo\n\n"), utili[y].nome);
					// fazer verificacao de paredes aqui - em falta
					int Posx;
					int Posy;
					do {
						Posx = 1 + (rand() % m->getLinhas());
						Posy = 1 + (rand() % m->getColunas());
					} while (m->VerificaParede(Posx,Posy) == true);

					jog->setPosX(Posx);
					jog->setPosY(Posy);
					m->NovoJogador(jog);
					PartilhaJogador(jog);
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
					int x1;
					int y1;
					do {
						x1 = 1 + (rand() % m->getLinhas());
						y1 = 1 + (rand() % m->getColunas());
					} while (m->VerificaParede(x1, y1) == true);

					jog->setPosX(x1);
					jog->setPosY(y1);
					m->NovoJogador(jog);
					PartilhaJogador(jog);
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
					_tprintf(TEXT("[Servidor] O cliente comeu um objeto\n"));
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
					_tprintf(TEXT("[Servidor] O cliente comeu um objeto\n"));
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
					_tprintf(TEXT("[Servidor] O cliente comeu um objeto\n"));
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
					_tprintf(TEXT("[Servidor] O cliente comeu um objeto\n"));
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
			res.comandoErrado = true;
			_tprintf(TEXT("[Servidor] O jogo ainda nao foi criado ou iniciado\n"));
			_tcscpy_s(res.comandoErr, 256, (TEXT("[Servidor] O jogo ainda nao foi criado ou iniciado\n")));
		}

		if (res.jogoCriado == true && res.jogoIniciado == true) {

			FazerMapa(jog);
			res.vida = jog->getVida();
			res.pontuacao = jog->getPontos();
			res.pedras = jog->getNpedras();
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

