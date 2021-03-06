// =============================================================================
// DialogBox.C											     
// ============================================================================= 
// Cria uma janela
// Usa um icon definido na aplica��o para a janela
// Mostra um menu com sub.menus pop-up e responde �s selec��es
// Usa teclas de atalho (Acelerator Keys) para esse menu
// Cria uma Dialog Box com PushButton, Listbox e EditBox

// ============================================================================
// In�cio do programa
// ============================================================================

#include <windows.h>
#include <tchar.h>		// Para chamada � fun��o "sprintf" 
#include <string.h>
#include <mbstring.h>
#include <wchar.h>
#include <stdio.h>


// ============================================================================
// resource.h
// File criada pelo VC++ com resources necess�rias ao
// programa (�cones, Menus, Teclas de atalho e Dialog Boxes, neste exemplo)
// ============================================================================
#include "resource.h"
#define TAM 255

#define MAXCLIENTES 5

#define PIPENOME TEXT("\\\\.\\pipe\\teste")
#define PIPENOME2 TEXT("\\\\.\\pipe\\teste2")


#ifdef UNICODE
#define tcout wcout
#define tcin wcin
#define tos wotstringstream
#define tfstream wifstream
#define tstring wstring
#define otstringstream wostringstream
#define itstringstream wistringstream
#define to_tstring to_wstring
#define tstringstream wstringstream
#else
#define tcout cout
#define tcin cin
#define tos otstringstream
#define tfstream ifstream
#define tstring string
#define otstringstream ostringstream
#define itstringstream istringstream
#define to_tstring to_string
#define tstringstream stringstream
#endif


HANDLE hPipe;
HANDLE hPipe2;
DWORD WINAPI Recebe(LPVOID param);
int Autenticacao(LPVOID param);
//void Envia(TCHAR &Comando);


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



struct resposta res;
static int ID_Cliente = 0;
int RES, resp = 0;
int jogo = 0;
TCHAR Comando[256];
TCHAR nome[25];
TCHAR pass[25];
DWORD n;
HWND pararecebe;
HBITMAP mp, m, m2, mc;
HDC hug;


// Vari�vel global hInstance usada para guardar "hInst" inicializada na fun��o
// WinMain(). "hInstance" � necess�ria no bloco WinProc() para lan�ar a Dialog
// box em DialogBox(...) 
HINSTANCE hInstance;

typedef int(*PonteiroFuncao)(TCHAR*, TCHAR*);

void Envia(TCHAR Comand[256]);

// Declara��o antecipada da fun��o de processamento da janela "WndProc()"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Declara��o antecipada da fun��o de processamento da DialogBox, "DialogProc()"
BOOL CALLBACK DialogJogar(HWND hWnd, UINT messg, WPARAM wp, LPARAM lParam);
// Mostrar numa dialog box cliente escolhido na ListBox e confirm�-lo
// Se sim, mostrar nome numa Edit Box
void ConfirmarItemSeleccionado(HWND);

// Mant�m a Edit Box actualizada com a selec��o da List Box
void ActualizaEditBox(HWND hw);

// Nome da janela / programa
TCHAR szProgName[] = TEXT("Interface cliente so2");

// Dados para uma List Box que faz parte da Dialog Box
struct fichas {
	TCHAR nome[40];		// nome
	unsigned bi;		// bilhete de identidade
	long saldo;			// saldo de conta corrente
};

#define NUM 7

// ============================================================================
// WinMain()
// NOTA: Ver declara��o de HACCEL, LoadAccelerators() e TranslateAccelerator()
//		 Estas s�o as modifica��es necess�rias para tratar as teclas de atalho
//		 para op��es do menu
// ============================================================================

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	HANDLE hThread;
	HWND hWnd;			// handler da janela (a gerar por CreateWindow())
	MSG lpMsg;			// Estrutura das mensagens
	WNDCLASSEX wcApp;	// Estrutura que define a classe da janela
	HACCEL hAccel;		// Handler da resource accelerators (teclas de atalho
						// do menu)
	wcApp.cbSize = sizeof(WNDCLASSEX);
	wcApp.hInstance = hInst;

	wcApp.lpszClassName = szProgName;
	wcApp.lpfnWndProc = WndProc;
	wcApp.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcApp.hIcon = LoadIcon(hInst, (LPCTSTR)IDI_ICON1);	// �con normal=Aplica��o do Windows

														//==============================================================================
														// �con definido pelo programador 
														//==============================================================================
														// Um �con definido pelo programador � uma "resource" do programa 
														// Para criar um icon fazer "Insert" "Resource" "Icon"
														// Em "wcApp.hIconSm=LoadIcon(..) indicar o nome do �con (neste caso ID_ICON1)
														// e o handler da inst�ncia actual "hInst" 
														//==============================================================================
	wcApp.hIconSm = LoadIcon(hInst, (LPCTSTR)IDI_ICON1);	// �con definido pelo
															// programador		
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);			// rato = "seta"
															//==============================================================================
															// Menu do programa
															//==============================================================================
															// Um menu � uma "resource" do programa 
															// O Windows cria uma resource para o menu com identifica��es para cada op��o
															// (ver "resources.h")
															// Em "wcApp.lpszMenuName..." indicar o ID do menu vis�vel em Resources
															//==============================================================================
	wcApp.lpszMenuName = (LPCTSTR)IDR_MENU2;		// Menu da janela		

	wcApp.cbClsExtra = 0;
	wcApp.cbWndExtra = 0;
	wcApp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	if (!RegisterClassEx(&wcApp))
		return(0);

	// ============================================================================
	// Guardar hInst na vari�vel global hInstance (inst�ncia do programa actual)
	// para uso na fun��o de processamento da janela WndProc()
	// ============================================================================
	hInstance = hInst;

	/*xMax = GetSystemMetrics(SM_CXSCREEN);
	yMax*/
	hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
		szProgName,				// Nome da janela e/ou programa
		TEXT("Liente do Jogo"),	// T�tulo da janela
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_VISIBLE | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,	// Estilo da janela 
		0,
		0,			// Posi��o x 		// Posi��o y 
		1000,			// Largura 
		600,			// Altura 
		(HWND)HWND_DESKTOP,	// handle da janela pai (HWND_DESKTOP para 1�)
		(HMENU)NULL,			// handle do menu (se tiver menu)
		(HINSTANCE)hInst,			// handle da inst�ncia actual (vem de WinMain())
		(LPSTR)NULL);			// N�o h� par�metros adicionais 

								//hWnd2 = CreateWindow(
								//	szProgName,				// Nome da janela e/ou programa
								//	TEXT("Segunda janela"),	// T�tulo da janela
								//	WS_OVERLAPPEDWINDOW,	// Estilo da janela 
								//	150,			// Posi��o x 
								//	150,			// Posi��o y 
								//	100,			// Largura 
								//	100,			// Altura 
								//	(HWND)hWnd,	// handle da janela pai (HWND_DESKTOP para 1�)
								//	(HMENU)NULL,			// handle do menu (se tiver menu)
								//	(HINSTANCE)hInst,			// handle da inst�ncia actual (vem de WinMain())
								//	(LPSTR)NULL);			// N�o h� par�metros adicionais 

								// ============================================================================
								// Carregar as defini��es das teclas aceleradoras (atalhos de op��es dos Menus)
								// LoadAccelerators(inst�ncia_programa, ID_da_resource_atalhos)
								// ============================================================================
	hAccel = LoadAccelerators(hInst, (LPCWSTR)IDR_ACCELERATOR1);


	ShowWindow(hWnd, nCmdShow);	// "hWnd"= handler da janela
								// "nCmdShow"= modo, par�metro de WinMain()
	UpdateWindow(hWnd);			// Refrescar a janela (gera WM_PAINT) 
								//ShowWindow(hWnd2, nCmdShow);
								//UpdateWindow(hWnd2);

	pararecebe = hWnd;
	// ============================================================================
	// Loop de Mensagens
	// Para usar as teclas aceleradoras do menu � necess�rio chamar a fun��o
	// TranslateAccelerator(handler_janela, handler_accelerators, pont_mensagem)
	// Se esta fun��o devolver falso, n�o foi premida tecla de acelera��o 
	// ============================================================================

	if (!WaitNamedPipe(PIPENOME, NMPWAIT_WAIT_FOREVER)) {
		//_tprintf(TEXT("[INFO] Conectando... '%s'\n"), PIPENOME);
		int respost = MessageBox(hWnd, (TEXT("[INFO] Conectando... '%s'\n"), PIPENOME), TEXT("Erro1"), MB_YESNO | MB_ICONQUESTION);
		if (respost == IDYES)
			PostQuitMessage(0);
	}

	hPipe = CreateFile(PIPENOME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	hPipe2 = CreateFile(PIPENOME2, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hPipe == NULL || hPipe2 == NULL) {
		int respost = MessageBox(hWnd, (TEXT("[INFO] Nao e possivel conetar o servidor %s, tente mais tarde.\n"), PIPENOME), TEXT("Erro2"), MB_YESNO | MB_ICONQUESTION);
		if (respost == IDYES)
			PostQuitMessage(0);				// Se YES terminar
											//_tprintf(TEXT("[INFO] Nao e possivel conetar o servidor %s, tente mais tarde.\n"), PIPENOME);
	}

	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Recebe, (LPVOID)hPipe2, 0, NULL);

	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		//if(!TranslateAccelerator(hWnd, hAccel, &lpMsg)){
		TranslateMessage(&lpMsg);		// Pr�-processamento da mensagem
		DispatchMessage(&lpMsg);		// Enviar a mensagem traduzida de volta
										//}									// ao Windows
	}
	return((int)lpMsg.wParam);	// Status = Par�metro "wParam" da estrutura "lpMsg"
}

//enviar msg para o servidor
void Envia(TCHAR Comand[256]) {

	WriteFile(hPipe, Comando, _tcslen(Comando)* sizeof(TCHAR), &n, NULL);
}


DWORD WINAPI Recebe(LPVOID param) {

	HANDLE hpipelocal = (HANDLE)param;
	BOOL ret;
	int i = 0, flag = 0, flag2 = 0, flag3 = 0;
	TCHAR buf[256];
	TCHAR frase[256];

	do {
		ReadFile(hpipelocal, &resp, sizeof(resp), &n, NULL);
	} while (resp==0);
		ReadFile(hpipelocal, buf, sizeof(buf), &n, NULL);
		//	buf[n / sizeof(TCHAR)] = '\0';

		ReadFile(hpipelocal, buf, sizeof(buf), &n, NULL);
	
	while (1) {

		ret = ReadFile(hpipelocal, &res, sizeof(struct resposta), &n, NULL);
		if (!ret || !n) {
			_tcscpy_s(buf, 256, "[Cliente ");
			wcscat_s(buf, 256, nome);
			_tcscpy_s(buf, 256, "] O servidor desligou-se\n\n");
			MessageBox(pararecebe, buf, TEXT("Servidor foi abaixo"), MB_OK);
			//_tprintf(TEXT("[Cliente -%s] O servidor desligou-se\n\n"), nome);
			break;
		}

		if (ID_Cliente == 0) {
			ID_Cliente = res.ID_Cliente;
		}
		if (res.JogadorLogado == TRUE) {

			if (res.jogoCriado == TRUE && flag == 0) {
				MessageBox(pararecebe, TEXT("[Servidor] O jogo ja foi criado\n"), TEXT("Jogo criado"), MB_OK);
				//_tprintf(TEXT("[Servidor] O jogo ja foi criado\n"));
				flag++;
			}
			if (res.jogoIniciado == TRUE && flag2 == 0) {
				MessageBox(pararecebe, TEXT("[Servidor] O jogo ja foi iniciado\n"), TEXT("Jogo iniciado"), MB_OK);
				//_tprintf(TEXT("[Servidor] O jogo ja foi iniciado\n"));
				flag2++;
				jogo = 1;
			}

			if (res.comandoErrado == TRUE) {
				_tcscpy_s(buf, 256, res.comandoErr);
				MessageBox(pararecebe, buf, TEXT("comando errado"), MB_OK);
				//_tprintf(TEXT("%s"), res.comandoErr);

			}

			if (res.jogoCriado == TRUE && res.jogoIniciado == TRUE) {
				InvalidateRect(pararecebe, NULL, FALSE);
				/*_tcscpy_s(buf, 256, "Vida do Jogador: ");
				wcscat_s(buf, 256, res.vida);
				_tcscpy_s(buf, 256, "\n Pontuacao do Jogador: ");
				wcscat_s(buf, 256, res.pontuacao);
				_tprintf(TEXT("Vida do Jogador: %d\n"), res.vida);
				_tprintf(TEXT("Pontuacao do Jogador: %d\n"), res.pontuacao);
				_tprintf(TEXT("%s"), res.frase);*/
				//jogo = 2;
			}
			if (flag == 1 && flag2 == 1) {
				InvalidateRect(pararecebe, NULL, FALSE);
				/*_tprintf(TEXT("[Servidor] O seu mapa:\n"));
				for (int ij = 0; ij < 13; ij++) {
				for (int ji = 0; ji < 13; ji++) {
				if (ij == 6 && ji == 6) {
				_tprintf(TEXT("|%d|"), res.mapa[ij][ji]);
				}
				else {
				_tprintf(TEXT("%d "), res.mapa[ij][ji]);
				}
				}
				_tprintf(TEXT("\n"));
				}
				_tprintf(TEXT("\n"));*/
			}

			//_tprintf(TEXT("[%s - Comandos]: "), nome);

		}

	}
}


BOOL CALLBACK DialogJogar(HWND hWnd, UINT messg, WPARAM wp, LPARAM lParam)
{
	//HANDLE hThread;

	//hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Recebe, (LPVOID)hPipe2, 0, NULL);
	switch (messg) {
	case WM_COMMAND:


		if (LOWORD(wp) == IDC_BUTTONPRE) {
			_tcscpy_s(Comando, 256, (TEXT("criarjogo pre")));
			//Comando[_tcslen(Comando) - 1] = '\0';
			res.ID_Cliente = ID_Cliente;
			Envia(Comando);

		}
		if (LOWORD(wp) == IDC_BUTTONRAN) {
			//dimens�es escolhidas pelo utilizador
			_tcscpy_s(Comando, 256, (TEXT("criarjogo ran")));
			//Comando[_tcslen(Comando) - 1] = '\0';
			res.ID_Cliente = ID_Cliente;
			Envia(Comando);
		}
		if (LOWORD(wp) == IDOK) {
			//jogar o jogo aparecer bitmaps
			_tcscpy_s(Comando, 256, (TEXT("jogar")));
			//Comando[_tcslen(Comando) - 1] = '\0';
			res.ID_Cliente = ID_Cliente;
			Envia(Comando);

			//voltar ao menu
			EndDialog(hWnd, 1);
			return TRUE;
		}
		if (LOWORD(wp) == IDC_BUTTONENTRAR) {
			//jogar o jogo aparecer bitmaps
			_tcscpy_s(Comando, 256, (TEXT("entrar jogo")));
			//Comando[_tcslen(Comando) - 1] = '\0';
			res.ID_Cliente = ID_Cliente;
			Envia(Comando);

			//voltar ao menu
			EndDialog(hWnd, 1);
			return TRUE;
		}
		if (LOWORD(wp) == IDCANCEL) {
			//voltar ao menu
			EndDialog(hWnd, 1);
			return TRUE;
		}
		//usar o  UpdateWindow(  _In_ HWND hWnd);


		break;
	default:
		break;
	}
	return FALSE;
}


BOOL CALLBACK DialogNova(HWND hWnd, UINT messg, WPARAM wp, LPARAM lParam)
{
	switch (messg) {
	case WM_COMMAND:
		if (LOWORD(wp) == IDOK) {
			EndDialog(hWnd, 1);
			return TRUE;
		}
		break;
	default:
		break;
	}
	return FALSE;
}

// ============================================================================
// FUN��O DE PROCESSAMENTO DA JANELA
// ============================================================================
BOOL CALLBACK Trata(HWND hWnd, UINT messg, WPARAM w, LPARAM lParam, int retor) {//Retorna FALSE se n�o tratar evento, TRUE se tratar o evento
	TCHAR nada[30] = TEXT("");
	switch (messg) {
	case WM_COMMAND:

		if (LOWORD(w) == IDOK) {
			GetDlgItemText(hWnd, IDC_EDIT1, nome, 30);
			GetDlgItemText(hWnd, IDC_EDIT2, pass, 30);
			if (_tcscmp(nome, nada) == 0 || _tcscmp(pass, nada) == 0) {
				int resposta = MessageBox(hWnd, TEXT("Intruduza uma conta valida"), TEXT("Sem sucesso"), MB_OK);
				if (resposta == IDYES)
					PostQuitMessage(0);				// Se for YES terminar
				break;
			}
			else {
				int passa = 0;
				Autenticacao(hWnd, hPipe2, nome, pass);
				if (resp == 1) {
					EndDialog(hWnd, 1);
					RES = 1;
					return TRUE;

				}
				else {
					int resposta = MessageBox(hWnd, TEXT("Intruduza uma conta valida"), TEXT("Sem sucesso"), MB_OK);
					if (resposta == IDYES)
						PostQuitMessage(0);				// Se for YES terminar
					break;
				}
			}
		}

		if (LOWORD(w) == IDCANCEL) {
			EndDialog(hWnd, 1);
			RES = 0;
			return FALSE;
		}

		break;
	case WM_INITDIALOG:

		break;
	case WM_CLOSE:
		EndDialog(hWnd, 1);
		RES = 0;
		return FALSE;
		break;
	default:
		break;
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, auxdc, hdctexto, auxchao;
	PAINTSTRUCT pt;
	HANDLE hThread;
	DWORD n;
	int resposta;					// Resposta a MessageBox
	HWND h1, h2;
	TCHAR nome2[25];
	static HMENU hMenu = NULL;
	static int ataque = 0, irparacima = 0, irparabaixo = 0, irparaesquerda = 0, irparaadireita = 0, sacapedra = 0;

	// Processamento das mensagens


	switch (messg)
	{

		//==============================================================================
		// Terminar e Processamentos default
		//==============================================================================
	case WM_CLOSE:
		if (MessageBox(hWnd, TEXT("Tem a certeza que quer sair do jogo?"), TEXT("Sair do jogo"), MB_OKCANCEL) == IDOK) {
			PostQuitMessage(0);
			break;
		}
		break;
		//==============================================================================
		// Resposta a op��es do Menu da janela
		// Clicar num menu gera a mensagem WM_COMMAND. Nesta mensagem:
		// O valor LOWORD(wParam) � o identificador das op��es do menu (ver o ficheiro 
		// "resource.h" que o VC++ criou). Os s�mbolos usados em seguida no "switch"  
		// est�o definidos em "resource.h"
		//==============================================================================
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_autent:

			DialogBox(hInstance, (LPCWSTR)MAKEINTRESOURCE(111), hWnd, (DLGPROC)Trata);
			//h1=CreateDialog(hInstance, (LPCWSTR) IDD_DIALOG4, hWnd, (DLGPROC) Trata);
			//h2=CreateDialog(hInstance, (LPCWSTR)IDD_DIALOG5, hWnd, (DLGPROC)Trata);
			//ShowWindow(h1, SW_SHOW);
			//ShowWindow(h2, SW_SHOW);
			if (RES == 1) {
				_tcscpy_s(nome2, 25, (TEXT("Bem vindo ")));
				wcscat_s(nome2, 25, nome);
				if (MessageBox(hWnd, nome2, TEXT("Autentica��o"), MB_OK) == IDOK) {
					//SendDlgItemMessage(hWnd, MAKEINTRESOURCE(40013), LB_ADDSTRING, 0, (LPARAM)nome);

					InsertMenuItem(IDR_MENU3, 1, TRUE, nome);
					hMenu = LoadMenu(NULL, IDR_MENU3);

					AppendMenu(hMenu, MF_STRING, IDR_MENU3, nome2);
					SetMenu(hWnd, hMenu);
				}
			}
			break;
		case ID_SAIR:
			resposta = MessageBox(hWnd, TEXT("Terminar o Programa?"), TEXT("Fim"), MB_YESNO | MB_ICONQUESTION);
			if (resposta == IDYES)
				PostQuitMessage(0);				// Se YES terminar
			break;
		case ID_AJUDA:
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG4), hWnd, (DLGPROC)DialogNova);
			break;
		case ID_LOGIN:

			//id�s relativos a segunda barra
		case ID_jogar: //id jogar da barra autentica��o 
					   //ver envia e resto
			DialogBox(hInstance, (LPCWSTR)MAKEINTRESOURCE(IDD_DIALOG5), hWnd, (DLGPROC)DialogJogar);
			break;
		case ID_Pontuacao:

			break;
		case ID_Ajudalogin:

			break;
		case ID_SAIRlogin:
			if (jogo == 1) {
				_tcscpy_s(Comando, 256, (TEXT("logout")));
				//Comando[_tcslen(Comando) - 1] = '\0';
				res.ID_Cliente = ID_Cliente;
				Envia(Comando);
				jogo = 2;
			}
			else {
				hMenu = LoadMenu(NULL, IDR_MENU2);
				SetMenu(hWnd, hMenu);
			}
			break;
		}
	case WM_PAINT:
		if (jogo == 0) {
			//desenhos antes do jogo
			hdc = BeginPaint(hWnd, &pt);

			EndPaint(hWnd, &pt);
		}
		else if (jogo == 2) {
			//apresnetar algumas coisas da jogada
			hdc = BeginPaint(hWnd, &pt);

			EndPaint(hWnd, &pt);
		}
		else {//fazer o mapa e o jogador
			hdc = BeginPaint(hWnd, &pt);
			HBRUSH fundo;
			int quemeojogador = (res.mapa[4][4]);

			int posixx = 0, posiyy = 0;
			for (int xu = 0; xu < 975; xu += 109) { //108
				for (int yu = 0; yu < 715; yu += 80) {//79
													   //ver que tipo de objeto

					if (res.mapa[posixx][posiyy] == 0) {//ch�o
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						BitBlt(hdc, xu, yu, 109, 80, auxdc, 0, 0, SRCCOPY); //ch�o

					}
					else if (res.mapa[posixx][posiyy] == 1) {//parede
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(174));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						BitBlt(hdc, xu, yu, 109, 80, auxdc, 0, 0, SRCCOPY); //parede

					}
					else if (res.mapa[posixx][posiyy] == 2) {//vitamina
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						BitBlt(hdc, xu, yu, 109, 80, auxdc, 0, 0, SRCCOPY); //ch�o
						DeleteDC(auxdc);												//---desenhar ch�o

						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(172));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, xu, yu, 48, 40, auxdc, 0, 0, 48, 40, RGB(255, 255, 255));//para o rebu�ado e vitamina

					}
					else if (res.mapa[posixx][posiyy] == 3) {//orange bull
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						BitBlt(hdc, xu, yu, 109, 80, auxdc, 0, 0, SRCCOPY); //ch�o
						DeleteDC(auxdc);											//---desenhar ch�o

						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(169));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, xu, yu, 88, 93, auxdc, 0, 0, 88, 93, RGB(255, 255, 255)); //para a garrafa

					}
					else if (res.mapa[posixx][posiyy] == 4) {//pedra
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						BitBlt(hdc, xu, yu, 109, 80, auxdc, 0, 0, SRCCOPY); //ch�o
						DeleteDC(auxdc);											//---desenhar ch�o

						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(170));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, xu, yu, 60, 60, auxdc, 0, 0, 60, 60, RGB(255, 255, 255));//para a pedra

					}
					else if (res.mapa[posixx][posiyy] == 5) {//rebu�ado
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						BitBlt(hdc, xu, yu, 109, 80, auxdc, 0, 0, SRCCOPY); //ch�o
						DeleteDC(auxdc);											//---desenhar ch�o

						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(171));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, xu, yu, 48, 40, auxdc, 0, 0, 48, 40, RGB(255, 255, 255));//para o rebu�ado e vitamina

					}
					//----------------------bitmaps para os jogadores
					else if (res.mapa[posixx][posiyy] == 6) {//jog no mapa batman
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						BitBlt(hdc, xu, yu, 109, 80, auxdc, 0, 0, SRCCOPY); //ch�o
						DeleteDC(auxdc);										//---desenhar ch�o

						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP18));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, xu, yu, 60, 105, auxdc, 0, 0, 60, 105, RGB(255, 255, 255));

					}
					else if (res.mapa[posixx][posiyy] == 7) {//jog no mapa flash
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						BitBlt(hdc, xu, yu, 109, 80, auxdc, 0, 0, SRCCOPY); //ch�o
						DeleteDC(auxdc);											//---desenhar ch�o

						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(137));
						fundo = CreatePatternBrush(mp);
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, xu, yu, 69, 111, auxdc, 0, 0, 69, 111, RGB(255, 255, 255));

					}
					else if (res.mapa[posixx][posiyy] == 8) {//jog no mapa supermen
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						BitBlt(hdc, xu, yu, 109, 80, auxdc, 0, 0, SRCCOPY); //ch�o
						DeleteDC(auxdc);												//---desenhar ch�o

						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(144));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, xu, yu, 114, 106, auxdc, 0, 0, 114, 106, RGB(255, 255, 255));

					}
					else if (res.mapa[posixx][posiyy] == 9) {//jog no mapa arrow
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						BitBlt(hdc, xu, yu, 109, 80, auxdc, 0, 0, SRCCOPY); //ch�o
						DeleteDC(auxdc);											//---desenhar ch�o

						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(149));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, xu, yu, 92, 122, auxdc, 0, 0, 92, 122, RGB(255, 255, 255));

					}
					else if (res.mapa[posixx][posiyy] == 10) {//jog no mapa wonder woman
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						BitBlt(hdc, xu, yu, 109, 80, auxdc, 0, 0, SRCCOPY); //ch�o
						DeleteDC(auxdc);											//---desenhar ch�o

						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(154));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, xu, yu, 66, 115, auxdc, 0, 0, 66, 115, RGB(255, 255, 255));

					}
					else if (res.mapa[posixx][posiyy] == 11) {//monstro
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						BitBlt(hdc, xu, yu, 109, 80, auxdc, 0, 0, SRCCOPY); //ch�o
						DeleteDC(auxdc);												//---desenhar ch�o

						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(167));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, xu, yu, 126, 126, auxdc, 0, 0, 126, 126, RGB(255, 255, 255));// para os primeiros dos monstros
					}
					else {//fora do mapa
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(175));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, xu, yu, 109, 80, auxdc, 0, 0, 109, 80, RGB(255, 255, 255));//para fora
					}


					DeleteDC(auxdc);
					DeleteDC(hug);
					posixx++;

				}
				posiyy++;
				posixx = 0;
			}
			DeleteDC(auxdc);
			DeleteDC(hug);

			mc = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
			auxchao = CreateCompatibleDC(hdc);
			SelectObject(auxchao, mc);

			//movimento do boneco do jogador....................^^...............................
			if (irparaadireita == 1) {//o boneco para a direita
				if (quemeojogador == 6) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(133));
				}
				else if (quemeojogador == 7) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(138));
				}
				else if (quemeojogador == 8) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(145));
				}
				else if (quemeojogador == 9) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(150));
				}
				else if (quemeojogador == 10) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(155));
				}

				hug = CreateCompatibleDC(hdc);
				SelectObject(hug, m);//pintar com o flsh a correr

				if (quemeojogador == 6) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 65, 98, hug, 0, 0, 65, 98, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 65, 98, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																			   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(132));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 60, 105, auxdc, 0, 0, 60, 105, RGB(255, 255, 255));
				}
				else if (quemeojogador == 7) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 20, 320, 156, 96, hug, 0, 0, 156, 96, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 156, 96, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					BitBlt(hdc, 436 - 25, 320, 156, 96, auxchao, 0, 0, SRCCOPY);

					//repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(137));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 69, 111, auxdc, 0, 0, 69, 111, RGB(255, 255, 255));
				}
				else if (quemeojogador == 8) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 84, 99, hug, 0, 0, 84, 99, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 84, 99, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																			   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(144));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 114, 106, auxdc, 0, 0, 114, 106, RGB(255, 255, 255));
				}
				else if (quemeojogador == 9) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 88, 114, hug, 0, 0, 88, 114, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 150, 150, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																				 //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(149));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 92, 122, auxdc, 0, 0, 92, 122, RGB(255, 255, 255));

				}
				else if (quemeojogador == 10) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 77, 99, hug, 0, 0, 77, 99, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 77, 99, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																			   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(154));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 66, 115, auxdc, 0, 0, 66, 115, RGB(255, 255, 255));
				}
				irparaadireita = 0;//repor
				DeleteDC(auxdc);
				DeleteDC(hug);
				DeleteDC(auxchao);

			}
			else if (irparaesquerda == 1) {//o boneco anda para esquerda 
				if (quemeojogador == 6) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(134));
				}
				else if (quemeojogador == 7) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(139));
				}
				else if (quemeojogador == 8) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(146));
				}
				else if (quemeojogador == 9) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(151));
				}
				else if (quemeojogador == 10) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(156));
				}

				hug = CreateCompatibleDC(hdc);
				SelectObject(hug, m);//pintar com o flsh a correr

				if (quemeojogador == 6) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 65, 98, hug, 0, 0, 65, 98, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 65, 98, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																			   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(132));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 60, 105, auxdc, 0, 0, 60, 105, RGB(255, 255, 255));
				}
				else if (quemeojogador == 7) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 74, 101, hug, 0, 0, 74, 101, RGB(255, 255, 255));
					Sleep(152);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 74, 101, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																				//repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(137));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 69, 111, auxdc, 0, 0, 69, 111, RGB(255, 255, 255));

				}
				else if (quemeojogador == 8) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 75, 111, hug, 0, 0, 75, 111, RGB(255, 255, 255));
					Sleep(152);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 84, 99, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																			   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(144));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 114, 106, auxdc, 0, 0, 114, 106, RGB(255, 255, 255));

				}
				else if (quemeojogador == 9) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 100, 106, hug, 0, 0, 100, 106, RGB(255, 255, 255));
					Sleep(140);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 150, 150, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																				 //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(149));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 92, 122, auxdc, 0, 0, 92, 122, RGB(255, 255, 255));


				}
				else if (quemeojogador == 10) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 80, 107, hug, 0, 0, 80, 107, RGB(255, 255, 255));
					Sleep(152);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 80, 107, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																				//repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(154));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 66, 115, auxdc, 0, 0, 66, 115, RGB(255, 255, 255));

				}
				irparaesquerda = 0;//repor
				DeleteDC(auxdc);
				DeleteDC(hug);
				DeleteDC(auxchao);

			}
			else if (irparabaixo == 1) {// o boneco anda para baixo 320>320
				if (quemeojogador == 6) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(133));
				}
				else if (quemeojogador == 7) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(141));
				}
				else if (quemeojogador == 8) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(145));
				}
				else if (quemeojogador == 9) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(150));
				}
				else if (quemeojogador == 10) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(155));
				}

				hug = CreateCompatibleDC(hdc);
				SelectObject(hug, m);//pintar com o flsh a correr

				if (quemeojogador == 6) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 65, 98, hug, 0, 0, 65, 98, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 65, 98, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																			   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(132));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 60, 105, auxdc, 0, 0, 60, 105, RGB(255, 255, 255));
				}
				else if (quemeojogador == 7) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436, 320 + 30, 96, 156, hug, 0, 0, 96, 156, RGB(255, 255, 255));
					Sleep(140);//esperar um pouco
					BitBlt(hdc, 436, 320 + 30, 96, 156, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					BitBlt(hdc, 436, 320 + 78, 96, 156, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																				//repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(137));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 69, 111, auxdc, 0, 0, 69, 111, RGB(255, 255, 255));
				}
				else if (quemeojogador == 8) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 84, 99, hug, 0, 0, 84, 99, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 84, 99, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																			   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(144));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 114, 106, auxdc, 0, 0, 114, 106, RGB(255, 255, 255));
				}
				else if (quemeojogador == 9) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 88, 114, hug, 0, 0, 88, 114, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 150, 150, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																				 //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(149));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 92, 122, auxdc, 0, 0, 92, 122, RGB(255, 255, 255));

				}
				else if (quemeojogador == 10) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 77, 99, hug, 0, 0, 77, 99, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 77, 99, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																			   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(154));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 66, 115, auxdc, 0, 0, 66, 115, RGB(255, 255, 255));
				}
				irparabaixo = 0;//repor
				DeleteDC(auxdc);
				DeleteDC(hug);
				DeleteDC(auxchao);
			}
			else if (irparacima == 1) {//o boneco anda para cima 320<320

				if (quemeojogador == 6) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(133));
				}
				else if (quemeojogador == 7) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(140));
				}
				else if (quemeojogador == 8) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(145));
				}
				else if (quemeojogador == 9) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(150));
				}
				else if (quemeojogador == 10) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(155));
				}

				hug = CreateCompatibleDC(hdc);
				SelectObject(hug, m);//pintar com o flsh a correr

				if (quemeojogador == 6) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 65, 98, hug, 0, 0, 65, 98, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 65, 98, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																			   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(132));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 60, 105, auxdc, 0, 0, 60, 105, RGB(255, 255, 255));
				}
				else if (quemeojogador == 7) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436, 320 + 30, 96, 156, hug, 0, 0, 96, 142, RGB(255, 255, 255));
					Sleep(140);//esperar um pouco

					BitBlt(hdc, 436, 320 + 83, 300, 300, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					BitBlt(hdc, 436, 320 + 23, 300, 300, auxchao, 0, 0, SRCCOPY);

					//repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(137));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 69, 111, auxdc, 0, 0, 69, 111, RGB(255, 255, 255));
				}
				else if (quemeojogador == 8) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 84, 99, hug, 0, 0, 84, 99, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 84, 99, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																			   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(144));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 114, 106, auxdc, 0, 0, 114, 106, RGB(255, 255, 255));
				}
				else if (quemeojogador == 9) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 88, 114, hug, 0, 0, 88, 114, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 150, 150, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																				 //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(149));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 92, 122, auxdc, 0, 0, 92, 122, RGB(255, 255, 255));

				}
				else if (quemeojogador == 10) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436 - 30, 320, 77, 99, hug, 0, 0, 77, 99, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 77, 99, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																			   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(154));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 66, 115, auxdc, 0, 0, 66, 115, RGB(255, 255, 255));
				}
				irparacima = 0; //repor
				DeleteDC(auxdc);
				DeleteDC(hug);
				DeleteDC(auxchao);

			}
			else if (ataque == 1) {//jogador ataca

				if (quemeojogador == 6) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(135));
				}
				else if (quemeojogador == 7) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(142));
				}
				else if (quemeojogador == 8) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(147));
				}
				else if (quemeojogador == 9) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(152));
				}
				else if (quemeojogador == 10) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(157));
				}

				hug = CreateCompatibleDC(hdc);
				SelectObject(hug, m);//pintar com o flsh a correr

				if (quemeojogador == 6) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436, 320, 92, 85, hug, 0, 0, 92, 85, RGB(255, 255, 255));
					Sleep(192);//esperar um pouco
					BitBlt(hdc, 436, 320, 92, 85, auxchao, 0, 0, SRCCOPY);//pintar com ch�o, a imagem � maior
					ataque = 0;//voltar a por o ataque a 0

							   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(132));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 60, 105, auxdc, 0, 0, 60, 105, RGB(255, 255, 255));
				}
				else if (quemeojogador == 7) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436, 320, 115, 106, hug, 0, 0, 115, 106, RGB(255, 255, 255));
					Sleep(192);//esperar um pouco
					BitBlt(hdc, 436, 320, 115, 106, auxchao, 0, 0, SRCCOPY);//pintar com ch�o, a imagem � maior
					ataque = 0;//voltar a por o ataque a 0

							   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(137));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 69, 111, auxdc, 0, 0, 69, 111, RGB(255, 255, 255));
				}
				else if (quemeojogador == 8) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436, 320, 71, 107, hug, 0, 0, 71, 107, RGB(255, 255, 255));
					Sleep(192);//esperar um pouco
					BitBlt(hdc, 436, 320, 71, 107, auxchao, 0, 0, SRCCOPY);//pintar com ch�o, a imagem � maior
					ataque = 0;//voltar a por o ataque a 0

							   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(144));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 114, 106, auxdc, 0, 0, 114, 106, RGB(255, 255, 255));
				}
				else if (quemeojogador == 9) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436, 320, 132, 117, hug, 0, 0, 132, 117, RGB(255, 255, 255));
					Sleep(152);//esperar um pouco
					BitBlt(hdc, 436, 320, 150, 150, auxchao, 0, 0, SRCCOPY);//pintar com ch�o, a imagem � maior
					ataque = 0;//voltar a por o ataque a 0

							   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(149));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 92, 122, auxdc, 0, 0, 92, 122, RGB(255, 255, 255));

				}
				else if (quemeojogador == 10) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436, 320, 87, 107, hug, 0, 0, 87, 107, RGB(255, 255, 255));
					Sleep(192);//esperar um pouco
					BitBlt(hdc, 436, 320, 87, 107, auxchao, 0, 0, SRCCOPY);//pintar com ch�o, a imagem � maior
					ataque = 0;//voltar a por o ataque a 0

							   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(154));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 66, 115, auxdc, 0, 0, 66, 115, RGB(255, 255, 255));
				}

				//voltar a desenhar normal que � feito fora do ciclo - vai ser feito aqui agora
				//----------------------bitmaps para os jogadores numero fazer daqui
				DeleteDC(auxdc);
				DeleteDC(hug);
				DeleteDC(auxchao);
			}
			else if (sacapedra == 1) {//jogador saca uma pedra

				if (quemeojogador == 6) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(133));
				}
				else if (quemeojogador == 7) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(138));
				}
				else if (quemeojogador == 8) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(145));
				}
				else if (quemeojogador == 9) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(150));
				}
				else if (quemeojogador == 10) {
					m = LoadBitmap(hInstance, MAKEINTRESOURCE(155));
				}

				hug = CreateCompatibleDC(hdc);
				SelectObject(hug, m);//pintar com o flsh a correr

				if (quemeojogador == 6) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436, 320, 65, 98, hug, 0, 0, 65, 98, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 65, 98, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																			   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(132));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 60, 105, auxdc, 0, 0, 60, 105, RGB(255, 255, 255));
				}
				else if (quemeojogador == 7) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436, 320, 156, 96, hug, 0, 0, 156, 96, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 156, 96, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					BitBlt(hdc, 436 - 25, 320, 156, 96, auxchao, 0, 0, SRCCOPY);

					//repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(137));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 69, 111, auxdc, 0, 0, 69, 111, RGB(255, 255, 255));
				}
				else if (quemeojogador == 8) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436, 320, 84, 99, hug, 0, 0, 84, 99, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 84, 99, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																			   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(144));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 114, 106, auxdc, 0, 0, 114, 106, RGB(255, 255, 255));
				}
				else if (quemeojogador == 9) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436, 320, 88, 114, hug, 0, 0, 88, 114, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 150, 150, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																				 //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(149));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 92, 122, auxdc, 0, 0, 92, 122, RGB(255, 255, 255));

				}
				else if (quemeojogador == 10) {
					BitBlt(hdc, 436, 320, 125, 125, auxchao, 0, 0, SRCCOPY);//pintar com ch�o
					TransparentBlt(hdc, 436, 320, 77, 99, hug, 0, 0, 77, 99, RGB(255, 255, 255));
					Sleep(148);//esperar um pouco
					BitBlt(hdc, 436 - 30, 320, 77, 99, auxchao, 0, 0, SRCCOPY);//pintar com ch�o

																			   //repintar
					mp = LoadBitmap(hInstance, MAKEINTRESOURCE(154));
					auxdc = CreateCompatibleDC(hdc);
					SelectObject(auxdc, mp);
					TransparentBlt(hdc, 436, 320, 66, 115, auxdc, 0, 0, 66, 115, RGB(255, 255, 255));
				}
				sacapedra = 0;
				DeleteDC(auxdc);
				DeleteDC(hug);
				DeleteDC(auxchao);
			}


			Sleep(12); //pequena pausa
			DeleteDC(auxdc);
			DeleteDC(hug);
			DeleteDC(auxchao);


			//fazer retangulo e cenas
			Rectangle(hdc, 995, 0, 1360, 595);

			int vidad = 10;
			//boneco, icon do jogo
			if (quemeojogador == 6) {
				m = LoadBitmap(hInstance, MAKEINTRESOURCE(136));
				hug = CreateCompatibleDC(hdc);
				SelectObject(hug, m);
				TransparentBlt(hdc, 1000, 0, 83, 87, hug, 0, 0, 83, 87, RGB(255, 255, 255));
			}
			else if (quemeojogador == 7) {
				m = LoadBitmap(hInstance, MAKEINTRESOURCE(143));
				hug = CreateCompatibleDC(hdc);
				SelectObject(hug, m);
				TransparentBlt(hdc, 1000, 0, 134, 127, hug, 0, 0, 134, 127, RGB(255, 255, 255));
			}
			else if (quemeojogador == 8) {
				m = LoadBitmap(hInstance, MAKEINTRESOURCE(148));
				hug = CreateCompatibleDC(hdc);
				SelectObject(hug, m);
				TransparentBlt(hdc, 1000, 0, 117, 119, hug, 0, 0, 117, 119, RGB(255, 255, 255));
			}
			else if (quemeojogador == 9) {
				m = LoadBitmap(hInstance, MAKEINTRESOURCE(153));
				hug = CreateCompatibleDC(hdc);
				SelectObject(hug, m);//desenhar o icon do boneco
				TransparentBlt(hdc, 1000, 0, 110, 110, hug, 0, 0, 110, 110, RGB(255, 255, 255));
			}
			else if (quemeojogador == 10) {
				m = LoadBitmap(hInstance, MAKEINTRESOURCE(158));
				hug = CreateCompatibleDC(hdc);
				SelectObject(hug, m);
				TransparentBlt(hdc, 1000, 0, 113, 118, hug, 0, 0, 113, 118, RGB(255, 255, 255));
			}

			DeleteDC(hug);
			DeleteDC(auxchao);
			DeleteDC(auxdc);
			DeleteDC(hdc);
			EndPaint(hWnd, &pt);

			//nomes e pontos e cenas
			hdctexto = GetDC(hWnd);

			if (quemeojogador == 6) {

				TextOut(hdctexto, 1000, 135, TEXT("Jogador 1- Batman: "), _tcslen(TEXT("Jogador 1- Batman: ")));
			}
			else if (quemeojogador == 7) {

				TextOut(hdctexto, 1000, 135, TEXT("Jogador 2- Flash: "), _tcslen(TEXT("Jogador 2-Flash: ")));
			}
			else if (quemeojogador == 8) {

				TextOut(hdctexto, 1000, 135, TEXT("Jogador 3- Supermen: "), _tcslen(TEXT("Jogador 3- Supermen: ")));
			}
			else if (quemeojogador == 9) {

				TextOut(hdctexto, 1000, 135, TEXT("Jogador 4- Arrow: "), _tcslen(TEXT("Jogador 4- Arrow: ")));
			}
			else if (quemeojogador == 10) {

				TextOut(hdctexto, 1000, 135, TEXT("Jogador 5- Wonder woman: "), _tcslen(TEXT("Jogador 5- Wonder woman: ")));
			}
			int paramet, adcion = 0, adcion2=0, adcion3 = 0, mudadelinha1=160, mudadelinha2=240;

			TextOut(hdctexto, 1000, 160, TEXT("Vida-> "), _tcslen(TEXT("Vida:  ")));
			m = LoadBitmap(hInstance, MAKEINTRESOURCE(179)); //boneco da vida
			hug = CreateCompatibleDC(hdc);
			SelectObject(hug, m);
			
			for (paramet = 0; paramet < res.vida; paramet++) {
				if (paramet==14 || paramet==28) {
					mudadelinha1 = mudadelinha1 + 21;
					adcion = 0;
				}
				TransparentBlt(hdc, 1038 + adcion, mudadelinha1, 19, 20, hug, 0, 0, 19, 20, RGB(255, 255, 255));
				adcion = adcion + 23;
				
			}

			TextOut(hdctexto, 1000, 240, TEXT("Pedras-> "), _tcslen(TEXT("Pedras:  ")));
			m = LoadBitmap(hInstance, MAKEINTRESOURCE(180)); //boneco da vida
			hug = CreateCompatibleDC(hdc);
			SelectObject(hug, m);
			for (paramet = 0; paramet < res.pedras; paramet++) {
				if (paramet == 12 || paramet == 26) {
					mudadelinha2 = mudadelinha2 + 21;
					adcion2 = 0;
				}
				TransparentBlt(hdc, 1055 + adcion2, mudadelinha2, 19, 17, hug, 0, 0, 19, 17, RGB(255, 255, 255));
				adcion2 = adcion2 + 23;
			}

			TextOut(hdctexto, 1000, 320, TEXT("Pontua��o-> "), _tcslen(TEXT("Pontua��o:  ")));
			m = LoadBitmap(hInstance, MAKEINTRESOURCE(181)); //boneco da vida
			hug = CreateCompatibleDC(hdc);
			SelectObject(hug, m);
			for (paramet = 0; paramet < res.pontuacao; paramet++) {
				TransparentBlt(hdc, 1060 + adcion3, 320, 18, 17, hug, 0, 0, 18, 17, RGB(255, 255, 255));
				adcion3 = adcion3 + 23;
			}
			adcion = 0;

			DeleteDC(hug);

			TextOut(hdctexto, 1000, 440, TEXT("Instru��es: "), _tcslen(TEXT("Instru��es:  ")));
			TextOut(hdctexto, 1000, 460, TEXT("Comandos:: SETAS para andar "), _tcslen(TEXT("Comandos-> SETAS para andar")));
			TextOut(hdctexto, 1000, 480, TEXT("ESPA�O para atacar"), _tcslen(TEXT("ESPA�O para atacar")));
			TextOut(hdctexto, 1000, 500, TEXT("SHIFT para por pedra na m�o "), _tcslen(TEXT("SHIFT para por pedra na m�o ")));
			TextOut(hdctexto, 1000, 565, res.frase, 15 * sizeof(TCHAR));

			ReleaseDC(hWnd, hdctexto);
		}

		break;
	case WM_KEYDOWN:
		//Imprimir
		//hdc = GetDC(hwnd);
		//TextOut(hdc, 436, 320, TEXT("&"), _tcslen(TEXT("&")));

		//ydown += 10;
		//		ReleaseDC(hwnd, hdc);
		if (wParam == VK_UP) {
			//320 -= 55;
			irparacima = 1;
			_tcscpy_s(Comando, 256, (TEXT("move cima")));
			//Comando[_tcslen(Comando) - 1] = '\0';
			res.ID_Cliente = ID_Cliente;
			Envia(Comando);
		}
		if (wParam == VK_DOWN) {
			//320 += 55;
			irparabaixo = 1;
			_tcscpy_s(Comando, 256, (TEXT("move baixo")));
			//Comando[_tcslen(Comando) - 1] = '\0';
			res.ID_Cliente = ID_Cliente;
			Envia(Comando);
		}
		if (wParam == VK_RIGHT) {
			//436 += 75;
			irparaadireita = 1;
			_tcscpy_s(Comando, 256, (TEXT("move direita")));
			//Comando[_tcslen(Comando) - 1] = '\0';
			res.ID_Cliente = ID_Cliente;
			Envia(Comando);
		}
		if (wParam == VK_LEFT) {
			//436 -= 75;
			irparaesquerda = 1;
			_tcscpy_s(Comando, 256, (TEXT("move esquerda")));
			//Comando[_tcslen(Comando) - 1] = '\0';
			res.ID_Cliente = ID_Cliente;
			Envia(Comando);
		}
		if (wParam == VK_SPACE) {//atacar
			ataque = 1;
			_tcscpy_s(Comando, 256, (TEXT("ataca")));
			//Comando[_tcslen(Comando) - 1] = '\0';
			res.ID_Cliente = ID_Cliente;
			Envia(Comando);
		}
		if (wParam == VK_SHIFT) {//pedra
			sacapedra = 1;
			_tcscpy_s(Comando, 256, (TEXT("pedra mao")));
			//Comando[_tcslen(Comando) - 1] = '\0';
			res.ID_Cliente = ID_Cliente;
			Envia(Comando);
		}

		InvalidateRect(hWnd, NULL, FALSE); //por o 1 a false para deixar de sintilar
		break;

		// Restantes mensagens t�m processamento default
	default:
		return(DefWindowProc(hWnd, messg, wParam, lParam));
		break;
	}
	return(0);
}


// ============================================================================
// FUN��O DE PROCESSAMENTO DA DIALOG BOX
// Esta fun��o deve retronar TRUE se processou a mensagem e FALSE se n�o a 
// processou (da� os "return 1" que terminam os diversos "case")
// ============================================================================
BOOL CALLBACK DialogAutenticacao(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	TCHAR login[30], passwd[30];
	HWND item;
	switch (messg) {
	case WM_INITDIALOG:
		SetDlgItemText(hWnd, IDC_EDIT1, TEXT("introduza aqui o seu login"));
		SetDlgItemText(hWnd, IDC_EDIT2, TEXT("introduza aqui a sua password"));
		SendDlgItemMessage(hWnd, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)TEXT("Jos�"));
		SendDlgItemMessage(hWnd, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)TEXT("Jo�o"));
		SendDlgItemMessage(hWnd, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)TEXT("Concei��o"));
		item = GetDlgItem(hWnd, IDC_EDIT1);
		wParam = IDC_EDIT1 | EN_SETFOCUS;
		SendMessage(hWnd, WM_COMMAND, (WPARAM)wParam, (LPARAM)item);
		return 1;
	case WM_COMMAND: //Clicou num controlo
		switch (LOWORD(wParam)) { //Que controlo?
		case IDCANCEL:
			EndDialog(hWnd, 0);
			return 1;

		case IDC_EDIT1:
			if (HIWORD(wParam) == EN_SETFOCUS) {
				item = GetDlgItem(hWnd, IDC_EDIT1);
				//SendMesFage(item, EM_SETSEL, (WPARAM)0, (LPARAM)_tcslen(TEXT("introduza aqui a sua password")));
			}
			return 1;
		case IDOK:
			//Buscar as 2 strings
			//Comparar com admin admin : em C++ ==, em C _tcscmp(str1,str2)
			//MessageBox: autenticado se sim, fecho a caixa 
			//Limpar as caixas de texto se n�o				
			GetDlgItemText(hWnd, IDC_EDIT1, login, 30);
			GetDlgItemText(hWnd, IDC_EDIT2, passwd, 30);
			if (_tcscmp(login, passwd) == 0) //Se forem iguais
				MessageBox(hWnd, TEXT("Aceite"), TEXT("Login"), MB_OK);
			else
				MessageBox(hWnd, TEXT("N�o aceite"), TEXT("Login"), MB_OK);

			return 1;
		case IDC_LIST1:
			if (HIWORD(wParam) == LBN_DBLCLK) {
				//Preencher caixa de texto com o texto selecionado da listbox
				int i = SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
				TCHAR login_temp[30];
				SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT, i, (LPARAM)login_temp);
				SetDlgItemText(hWnd, IDC_EDIT1, (LPCWSTR)login_temp);

			}
			return 1;
		}
	}
	return 0;

}

// ******************************************************************
// Fun��o "ActualizaEditBox":
// Escreve na Edit Box o nome do cliente actualmente seleccionado na
// List Box
// ******************************************************************
void ActualizaEditBox(HWND hw) {
	TCHAR str[TAM];
	int i;

	//Verifica que elemento (�ndice) est� seleccionado
	i = (int)SendDlgItemMessage(hw, IDC_LIST1, LB_GETCURSEL, 0, 0);
	//Ler a palavra na List Box correspondente ao �ndice
	SendDlgItemMessage(hw, IDC_LIST1, LB_GETTEXT, i, (LPARAM)str);
	//Coloc�-la na Edit Box
	SetDlgItemText(hw, IDC_EDIT1, str);
}

int Autenticacao(HWND hnd, LPVOID param, TCHAR nom[25], TCHAR pa[25]) {

	HANDLE hpipelocal = (HANDLE)param;
	DWORD n, p;
	BOOL ret;
	int i = 0, retorn;
	TCHAR buf[256];
	TCHAR frase[256];


	//autentica��o do utilizador - nome e pass 

	//do {
	//_tprintf(TEXT("[Autentica��o - coloque o nome]: "));
	//fflush(stdin);
	//nom[_tcslen(nom) - 1] = '\0';
	wcscpy_s(Comando, 256, nom);
	//_fgetts(Comando, 256, nom);
	wcscpy_s(nome, 25, Comando);
	res.ID_Cliente = ID_Cliente;
	WriteFile(hPipe, Comando, _tcslen(Comando) * sizeof(TCHAR), &n, NULL);

	//pa[_tcslen(pa) - 1] = '\0';
	wcscpy_s(Comando, 256, pa);
	//_fgetts(Comando, 256, pa);
	wcscpy_s(pass, 25, Comando);
	res.ID_Cliente = ID_Cliente;
	WriteFile(hPipe, Comando, _tcslen(Comando) * sizeof(TCHAR), &p, NULL);

	//ReadFile(hpipelocal, &retorn, sizeof(retorn), &n, NULL);
	//if (retorn == 0) {
	//_tprintf(TEXT("[Servidor] Essa conta j� esta em uso! Entre com outra conta\n"));
	//MessageBox(hnd, TEXT("Intruduza uma conta valida mas isro � assim"), TEXT("Sem sucesso"), MB_OK);

	//}

	return 1;
	//} while (resp != 1);
}
