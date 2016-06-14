// =============================================================================
// DialogBox.C											     
// ============================================================================= 
// Cria uma janela
// Usa um icon definido na aplicação para a janela
// Mostra um menu com sub.menus pop-up e responde às selecções
// Usa teclas de atalho (Acelerator Keys) para esse menu
// Cria uma Dialog Box com PushButton, Listbox e EditBox

// ============================================================================
// Início do programa
// ============================================================================

#include <windows.h>
#include <tchar.h>		// Para chamada à função "sprintf" 
#include <string.h>
#include <mbstring.h>
#include <wchar.h>
#include <stdio.h>


// ============================================================================
// resource.h
// File criada pelo VC++ com resources necessárias ao
// programa (Ícones, Menus, Teclas de atalho e Dialog Boxes, neste exemplo)
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
	BOOL JogadorLogado;
	BOOL jogoCriado;
	BOOL jogoIniciado;
	BOOL comandoErrado;
	TCHAR frase[256];
	TCHAR comandoErr[256];
	char nome[50];
	int mapa[13][13];
	int numero;
};


struct resposta res;
static int ID_Cliente = 0;
int RES, resp=0;
int jogo = 0;
TCHAR Comando[256];
TCHAR nome[25];
TCHAR pass[25];
DWORD n;
HWND pararecebe;
HBITMAP mp, m;
HDC hug;


// Variável global hInstance usada para guardar "hInst" inicializada na função
// WinMain(). "hInstance" é necessária no bloco WinProc() para lançar a Dialog
// box em DialogBox(...) 
HINSTANCE hInstance;

typedef int (*PonteiroFuncao)(TCHAR*, TCHAR*);

void Envia(TCHAR Comand[256]);

// Declaração antecipada da função de processamento da janela "WndProc()"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Declaração antecipada da função de processamento da DialogBox, "DialogProc()"
BOOL CALLBACK DialogJogar(HWND hWnd, UINT messg, WPARAM wp, LPARAM lParam);
// Mostrar numa dialog box cliente escolhido na ListBox e confirmá-lo
// Se sim, mostrar nome numa Edit Box
void ConfirmarItemSeleccionado(HWND);

// Mantém a Edit Box actualizada com a selecção da List Box
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
// NOTA: Ver declaração de HACCEL, LoadAccelerators() e TranslateAccelerator()
//		 Estas são as modificações necessárias para tratar as teclas de atalho
//		 para opções do menu
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
	wcApp.hIcon = LoadIcon(hInst, (LPCTSTR)IDI_ICON1);	// ícon normal=Aplicação do Windows

//==============================================================================
// Ícon definido pelo programador 
//==============================================================================
// Um ícon definido pelo programador é uma "resource" do programa 
// Para criar um icon fazer "Insert" "Resource" "Icon"
// Em "wcApp.hIconSm=LoadIcon(..) indicar o nome do ícon (neste caso ID_ICON1)
// e o handler da instância actual "hInst" 
//==============================================================================
	wcApp.hIconSm = LoadIcon(hInst, (LPCTSTR) IDI_ICON1);	// ícon definido pelo
															// programador		
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);			// rato = "seta"
//==============================================================================
// Menu do programa
//==============================================================================
// Um menu é uma "resource" do programa 
// O Windows cria uma resource para o menu com identificações para cada opção
// (ver "resources.h")
// Em "wcApp.lpszMenuName..." indicar o ID do menu visível em Resources
//==============================================================================
	wcApp.lpszMenuName = (LPCTSTR)IDR_MENU2;		// Menu da janela		
	
	wcApp.cbClsExtra = 0;				
	wcApp.cbWndExtra = 0;				
	wcApp.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);	

	if (!RegisterClassEx(&wcApp))
		return(0);

// ============================================================================
// Guardar hInst na variável global hInstance (instância do programa actual)
// para uso na função de processamento da janela WndProc()
// ============================================================================
	hInstance=hInst;
	
	/*xMax = GetSystemMetrics(SM_CXSCREEN);
	yMax*/
	hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
		szProgName,				// Nome da janela e/ou programa
		TEXT("Liente do Jogo"),	// Título da janela
		WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME| WS_VISIBLE| WS_MAXIMIZEBOX | WS_MINIMIZEBOX,	// Estilo da janela 
		0,
		0,			// Posição x 		// Posição y 
		1000,			// Largura 
		600,			// Altura 
		(HWND) HWND_DESKTOP,	// handle da janela pai (HWND_DESKTOP para 1ª)
		(HMENU) NULL,			// handle do menu (se tiver menu)
		(HINSTANCE) hInst,			// handle da instância actual (vem de WinMain())
		(LPSTR) NULL);			// Não há parâmetros adicionais 

	//hWnd2 = CreateWindow(
	//	szProgName,				// Nome da janela e/ou programa
	//	TEXT("Segunda janela"),	// Título da janela
	//	WS_OVERLAPPEDWINDOW,	// Estilo da janela 
	//	150,			// Posição x 
	//	150,			// Posição y 
	//	100,			// Largura 
	//	100,			// Altura 
	//	(HWND)hWnd,	// handle da janela pai (HWND_DESKTOP para 1ª)
	//	(HMENU)NULL,			// handle do menu (se tiver menu)
	//	(HINSTANCE)hInst,			// handle da instância actual (vem de WinMain())
	//	(LPSTR)NULL);			// Não há parâmetros adicionais 

// ============================================================================
// Carregar as definições das teclas aceleradoras (atalhos de opções dos Menus)
// LoadAccelerators(instância_programa, ID_da_resource_atalhos)
// ============================================================================
	hAccel=LoadAccelerators(hInst, (LPCWSTR) IDR_ACCELERATOR1);


	ShowWindow(hWnd, nCmdShow);	// "hWnd"= handler da janela
								// "nCmdShow"= modo, parâmetro de WinMain()
	UpdateWindow(hWnd);			// Refrescar a janela (gera WM_PAINT) 
	//ShowWindow(hWnd2, nCmdShow);
	//UpdateWindow(hWnd2);

	pararecebe = hWnd;
// ============================================================================
// Loop de Mensagens
// Para usar as teclas aceleradoras do menu é necessário chamar a função
// TranslateAccelerator(handler_janela, handler_accelerators, pont_mensagem)
// Se esta função devolver falso, não foi premida tecla de aceleração 
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

	while (GetMessage(&lpMsg,NULL,0,0)) {	
		//if(!TranslateAccelerator(hWnd, hAccel, &lpMsg)){
			TranslateMessage(&lpMsg);		// Pré-processamento da mensagem
			DispatchMessage(&lpMsg);		// Enviar a mensagem traduzida de volta
		//}									// ao Windows
	}
	return((int)lpMsg.wParam);	// Status = Parâmetro "wParam" da estrutura "lpMsg"
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
	
	
	
		ReadFile(hpipelocal, buf, sizeof(buf), &n, NULL);
		buf[n / sizeof(TCHAR)] = '\0';
	
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
			if (res.numero == 1) {
				resp = 1;
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
					jogo = 1;
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
	switch (messg){
	case WM_COMMAND:
		

		if (LOWORD(wp) == IDC_BUTTONPRE) {
			_tcscpy_s(Comando, 256, (TEXT("criarjogo pre")));
			//Comando[_tcslen(Comando) - 1] = '\0';
			res.ID_Cliente = ID_Cliente;
			Envia(Comando);
			
		}
		if (LOWORD(wp) == IDC_BUTTONRAN) {
			//dimensões escolhidas pelo utilizador
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
// FUNÇÂO DE PROCESSAMENTO DA JANELA
// ============================================================================
BOOL CALLBACK Trata(HWND hWnd, UINT messg, WPARAM w, LPARAM lParam, int retor){//Retorna FALSE se não tratar evento, TRUE se tratar o evento
	TCHAR nada[30]=TEXT("");
	switch(messg){
	case WM_COMMAND:
		
		if (LOWORD(w) == IDOK) {
			GetDlgItemText(hWnd, IDC_EDIT1, nome, 30);
			GetDlgItemText(hWnd, IDC_EDIT2, pass, 30);
			if(_tcscmp(nome, nada) == 0 || _tcscmp(pass, nada) == 0){
				int resposta = MessageBox(hWnd, TEXT("Intruduza uma conta valida"), TEXT("Sem sucesso"), MB_OK);
				if (resposta == IDYES)
					PostQuitMessage(0);				// Se for YES terminar
				break;
			}
			else {
				int passa=0;
				Autenticacao(hWnd, hPipe2, nome, pass);
				passa = resp;
				if(passa==1) {
				EndDialog(hWnd, 1);
				RES = 1;
				return TRUE;

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
	HDC hdc, auxdc, hdctexto;
	PAINTSTRUCT pt;
	HANDLE hThread;
	DWORD n;
	int resposta;					// Resposta a MessageBox
	HWND h1, h2;
	TCHAR nome2[25];
	static HMENU hMenu = NULL;
	static int ataque = 0, irparacima = 0, irparabaixo = 0, irparaesquerda = 0, irparaadireita = 0;
	
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
// Resposta a opções do Menu da janela
// Clicar num menu gera a mensagem WM_COMMAND. Nesta mensagem:
// O valor LOWORD(wParam) é o identificador das opções do menu (ver o ficheiro 
// "resource.h" que o VC++ criou). Os símbolos usados em seguida no "switch"  
// estão definidos em "resource.h"
//==============================================================================
	case WM_COMMAND: 
		switch (LOWORD(wParam)) 
		{
		case ID_autent:
			
			DialogBox(hInstance, (LPCWSTR) MAKEINTRESOURCE(111), hWnd, (DLGPROC)Trata);
			//h1=CreateDialog(hInstance, (LPCWSTR) IDD_DIALOG4, hWnd, (DLGPROC) Trata);
			//h2=CreateDialog(hInstance, (LPCWSTR)IDD_DIALOG5, hWnd, (DLGPROC)Trata);
			//ShowWindow(h1, SW_SHOW);
			//ShowWindow(h2, SW_SHOW);
			if (RES == 1) {
				_tcscpy_s(nome2, 25, (TEXT("Bem vindo ")));
				wcscat_s(nome2, 25, nome);
				if (MessageBox(hWnd, nome2, TEXT("Autenticação"), MB_OK) == IDOK) {
					//SendDlgItemMessage(hWnd, MAKEINTRESOURCE(40013), LB_ADDSTRING, 0, (LPARAM)nome);

					InsertMenuItem(IDR_MENU3, 1, TRUE, nome);
					hMenu = LoadMenu(NULL, IDR_MENU3);

					AppendMenu(hMenu, MF_STRING, IDR_MENU3, nome2);
					SetMenu(hWnd, hMenu);
				}
			}
			break;
		case ID_SAIR:
			resposta=MessageBox(hWnd,TEXT("Terminar o Programa?"), TEXT("Fim"), MB_YESNO|MB_ICONQUESTION);
			if (resposta==IDYES)
				PostQuitMessage(0);				// Se YES terminar
			break;
		case ID_AJUDA:	
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG4), hWnd, (DLGPROC)DialogNova);
			break;
		case ID_LOGIN:
		
		//id´s relativos a segunda barra
		case ID_jogar: //id jogar da barra autenticação 
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
			else if(jogo==2){
				//apresnetar algumas coisas da jogada
				hdc = BeginPaint(hWnd, &pt);

				EndPaint(hWnd, &pt);
			}
			else {//fazer o mapa e o jogador
				hdc = BeginPaint(hWnd, &pt);
				HBRUSH fundo;
				int quemeojogador = ((res.mapa[6][6])-5);
				
				int posixx = 0, posiyy = 0;
				for (int xu = 0; xu <= 975; xu += 75) {
					for (int yu = 0; yu <= 715; yu += 55) {
						//ver que tipo de objeto

						if (res.mapa[posixx][posiyy] == 0) {//chão
							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							BitBlt(hdc, xu, yu, 75, 55, auxdc, 0, 0, SRCCOPY); //chão

						}
						else if(res.mapa[posixx][posiyy] == 1) {//parede
							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(174));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							BitBlt(hdc, xu, yu, 75, 55, auxdc, 0, 0, SRCCOPY); //parede

						}
						else if (res.mapa[posixx][posiyy] == 2) {//vitamina
							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							BitBlt(hdc, xu, yu, 75, 55, auxdc, 0, 0, SRCCOPY); //chão
							//---desenhar chão

							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(172));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							TransparentBlt(hdc, xu, yu, 48, 40, auxdc, 0, 0, 48, 40, RGB(255, 255, 255));//para o rebuçado e vitamina

						}
						else if (res.mapa[posixx][posiyy] == 3) {//orange bull
							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							BitBlt(hdc, xu, yu, 75, 55, auxdc, 0, 0, SRCCOPY); //chão
																			   //---desenhar chão

							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(169));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							TransparentBlt(hdc, xu, yu, 88, 93, auxdc, 0, 0, 88, 93, RGB(255, 255, 255)); //para a garrafa

						}
						else if (res.mapa[posixx][posiyy] == 4) {//pedra
							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							BitBlt(hdc, xu, yu, 75, 55, auxdc, 0, 0, SRCCOPY); //chão
																			   //---desenhar chão

							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(170));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							TransparentBlt(hdc, xu, yu, 60, 60, auxdc, 0, 0, 60, 60, RGB(255, 255, 255));//para a pedra

						}
						else if (res.mapa[posixx][posiyy] == 5) {//rebuçado
							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							BitBlt(hdc, xu, yu, 75, 55, auxdc, 0, 0, SRCCOPY); //chão
																			   //---desenhar chão

							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(171));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							TransparentBlt(hdc, xu, yu, 48, 40, auxdc, 0, 0, 48, 40, RGB(255, 255, 255));//para o rebuçado e vitamina

						}
						//----------------------bitmaps para os jogadores
						else if (res.mapa[posixx][posiyy] == 6) {//jog no mapa batman
							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							BitBlt(hdc, xu, yu, 75, 55, auxdc, 0, 0, SRCCOPY); //chão
																			   //---desenhar chão

							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(132));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							TransparentBlt(hdc, xu, yu, 60, 105, hug, 0, 0, 60, 105, RGB(255, 255, 255));

						}
						else if (res.mapa[posixx][posiyy] == 7) {//jog no mapa flash
							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							BitBlt(hdc, xu, yu, 75, 55, auxdc, 0, 0, SRCCOPY); //chão
																			   //---desenhar chão

							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(137));
							fundo = CreatePatternBrush(mp);
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							TransparentBlt(hdc, 450, 330, 69, 111, hug, 0, 0, 69, 111, RGB(255, 255, 255));

						}
						else if (res.mapa[posixx][posiyy] == 8) {//jog no mapa supermen
							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							BitBlt(hdc, xu, yu, 75, 55, auxdc, 0, 0, SRCCOPY); //chão
																			   //---desenhar chão

							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(144));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							TransparentBlt(hdc, 450, 330, 114, 106, hug, 0, 0, 114, 106, RGB(255, 255, 255));

						}
						else if (res.mapa[posixx][posiyy] == 9) {//jog no mapa arrow
							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							BitBlt(hdc, xu, yu, 75, 55, auxdc, 0, 0, SRCCOPY); //chão
																			   //---desenhar chão

							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(149));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							TransparentBlt(hdc, 450, 330, 92, 122, hug, 0, 0, 92, 122, RGB(255, 255, 255));

						}
						else if (res.mapa[posixx][posiyy] == 10) {//jog no mapa wonder woman
							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							BitBlt(hdc, xu, yu, 75, 55, auxdc, 0, 0, SRCCOPY); //chão
																			   //---desenhar chão

							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(154));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							TransparentBlt(hdc, 450, 330, 66, 115, hug, 0, 0, 66, 115, RGB(255, 255, 255));

						}
						else if (res.mapa[posixx][posiyy] == 11) {//monstro
							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(173));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							BitBlt(hdc, xu, yu, 75, 55, auxdc, 0, 0, SRCCOPY); //chão
																			   //---desenhar chão

							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(167));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							TransparentBlt(hdc, xu, yu, 126, 126, auxdc, 0, 0, 126, 126, RGB(255, 255, 255));// para os primeiros dos monstros
						}
						else {//fora do mapa
							mp = LoadBitmap(hInstance, MAKEINTRESOURCE(175));
							auxdc = CreateCompatibleDC(hdc);
							SelectObject(auxdc, mp);
							TransparentBlt(hdc, xu, yu, 75, 55, auxdc, 0, 0, 75, 55, RGB(255, 255, 255));//para fora
						}


						DeleteDC(auxdc);
						posixx++;

					}
					posiyy++;
					posixx = 0;
				}


				//movimento do boneco do jogador....................^^...............................
				if(irparaadireita==1) {//o boneco para a direita
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
						TransparentBlt(hdc, 450 - 30, 330, 65, 98, hug, 0, 0, 65, 98, RGB(255, 255, 255));
						Sleep(148);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 65, 98, auxdc, 0, 0, SRCCOPY);//pintar com chão

						//repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(132));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 60, 105, hug, 0, 0, 60, 105, RGB(255, 255, 255));
					}
					else if (quemeojogador == 7) {
						TransparentBlt(hdc, 450 - 30, 330, 156, 96, hug, 0, 0, 156, 96, RGB(255, 255, 255));
						Sleep(148);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 156, 96, auxdc, 0, 0, SRCCOPY);//pintar com chão
						BitBlt(hdc, 450 - 25, 330, 156, 96, auxdc, 0, 0, SRCCOPY);

						//repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(137));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 69, 111, hug, 0, 0, 69, 111, RGB(255, 255, 255));
					}
					else if (quemeojogador == 8) {
						TransparentBlt(hdc, 450 - 30, 330, 84, 99, hug, 0, 0, 84, 99, RGB(255, 255, 255));
						Sleep(148);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 84, 99, auxdc, 0, 0, SRCCOPY);//pintar com chão

																				 //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(144));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 114, 106, hug, 0, 0, 114, 106, RGB(255, 255, 255));
					}
					else if (quemeojogador == 9) {
						TransparentBlt(hdc, 450 - 30, 330, 88, 114, hug, 0, 0, 88, 114, RGB(255, 255, 255));
						Sleep(148);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 150, 150, auxdc, 0, 0, SRCCOPY);//pintar com chão

																				   //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(149));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 92, 122, hug, 0, 0, 92, 122, RGB(255, 255, 255));

					}
					else if (quemeojogador == 10) {
						TransparentBlt(hdc, 450 - 30, 330, 77, 99, hug, 0, 0, 77, 99, RGB(255, 255, 255));
						Sleep(148);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 77, 99, auxdc, 0, 0, SRCCOPY);//pintar com chão

																				 //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(154));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 66, 115, hug, 0, 0, 66, 115, RGB(255, 255, 255));
					}
					irparaadireita = 0;//repor

				}
				else if (irparaesquerda==1) {//o boneco anda para esquerda 
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
						TransparentBlt(hdc, 450 - 30, 330, 65, 98, hug, 0, 0, 65, 98, RGB(255, 255, 255));
						Sleep(148);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 65, 98, auxdc, 0, 0, SRCCOPY);//pintar com chão

																				 //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(132));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 60, 105, hug, 0, 0, 60, 105, RGB(255, 255, 255));
					}
					else if (quemeojogador == 7) {
						TransparentBlt(hdc, 450 - 30, 330, 74, 101, hug, 0, 0, 74, 101, RGB(255, 255, 255));
						Sleep(152);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 74, 101, auxdc, 0, 0, SRCCOPY);//pintar com chão

																						//repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(137));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 69, 111, hug, 0, 0, 69, 111, RGB(255, 255, 255));

					}
					else if (quemeojogador == 8) {
						TransparentBlt(hdc, 450 - 30, 330, 75, 111, hug, 0, 0, 75, 111, RGB(255, 255, 255));
						Sleep(152);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 84, 99, auxdc, 0, 0, SRCCOPY);//pintar com chão

																					   //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(144));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 114, 106, hug, 0, 0, 114, 106, RGB(255, 255, 255));

					}
					else if (quemeojogador == 9) {
						TransparentBlt(hdc, 450 - 30, 330, 100, 106, hug, 0, 0, 100, 106, RGB(255, 255, 255));
						Sleep(140);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 150, 150, auxdc, 0, 0, SRCCOPY);//pintar com chão

																						 //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(149));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 92, 122, hug, 0, 0, 92, 122, RGB(255, 255, 255));


					}
					else if (quemeojogador == 10) {
						TransparentBlt(hdc, 450 - 30, 330, 80, 107, hug, 0, 0, 80, 107, RGB(255, 255, 255));
						Sleep(152);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 80, 107, auxdc, 0, 0, SRCCOPY);//pintar com chão

																						//repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(154));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 66, 115, hug, 0, 0, 66, 115, RGB(255, 255, 255));

					}
					irparaesquerda = 0;//repor

				}
				else if (irparabaixo==1) {// o boneco anda para baixo 330>330
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
						TransparentBlt(hdc, 450 - 30, 330, 65, 98, hug, 0, 0, 65, 98, RGB(255, 255, 255));
						Sleep(148);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 65, 98, auxdc, 0, 0, SRCCOPY);//pintar com chão

																				 //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(132));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 60, 105, hug, 0, 0, 60, 105, RGB(255, 255, 255));
					}
					else if (quemeojogador == 7) {
						TransparentBlt(hdc, 450, 330 + 30, 96, 156, hug, 0, 0, 96, 156, RGB(255, 255, 255));
						Sleep(140);//esperar um pouco
						BitBlt(hdc, 450, 330 + 30, 96, 156, auxdc, 0, 0, SRCCOPY);//pintar com chão
						BitBlt(hdc, 450, 330 + 78, 96, 156, auxdc, 0, 0, SRCCOPY);//pintar com chão

																						//repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(137));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 69, 111, hug, 0, 0, 69, 111, RGB(255, 255, 255));
					}
					else if (quemeojogador == 8) {
						TransparentBlt(hdc, 450 - 30, 330, 84, 99, hug, 0, 0, 84, 99, RGB(255, 255, 255));
						Sleep(148);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 84, 99, auxdc, 0, 0, SRCCOPY);//pintar com chão

																				 //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(144));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 114, 106, hug, 0, 0, 114, 106, RGB(255, 255, 255));
					}
					else if (quemeojogador == 9) {
						TransparentBlt(hdc, 450 - 30, 330, 88, 114, hug, 0, 0, 88, 114, RGB(255, 255, 255));
						Sleep(148);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 150, 150, auxdc, 0, 0, SRCCOPY);//pintar com chão

									//repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(149));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 92, 122, hug, 0, 0, 92, 122, RGB(255, 255, 255));

					}
					else if (quemeojogador == 10) {
						TransparentBlt(hdc, 450 - 30, 330, 77, 99, hug, 0, 0, 77, 99, RGB(255, 255, 255));
						Sleep(148);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 77, 99, auxdc, 0, 0, SRCCOPY);//pintar com chão

																				 //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(154));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 66, 115, hug, 0, 0, 66, 115, RGB(255, 255, 255));
					}
					irparabaixo = 0;//repor

				}
				else if (irparacima==1) {//o boneco anda para cima 330<330

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
						TransparentBlt(hdc, 450 - 30, 330, 65, 98, hug, 0, 0, 65, 98, RGB(255, 255, 255));
						Sleep(148);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 65, 98, auxdc, 0, 0, SRCCOPY);//pintar com chão

																				 //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(132));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 60, 105, hug, 0, 0, 60, 105, RGB(255, 255, 255));
					}
					else if (quemeojogador == 7) {
						TransparentBlt(hdc, 450, 330 + 30, 96, 156, hug, 0, 0, 96, 142, RGB(255, 255, 255));
						Sleep(140);//esperar um pouco
						BitBlt(hdc, 450, 330 + 83, 300, 300, auxdc, 0, 0, SRCCOPY);//pintar com chão
						BitBlt(hdc, 450, 330 + 23, 300, 300, auxdc, 0, 0, SRCCOPY);

						//repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(137));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 69, 111, hug, 0, 0, 69, 111, RGB(255, 255, 255));
					}
					else if (quemeojogador == 8) {
						TransparentBlt(hdc, 450 - 30, 330, 84, 99, hug, 0, 0, 84, 99, RGB(255, 255, 255));
						Sleep(148);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 84, 99, auxdc, 0, 0, SRCCOPY);//pintar com chão

																				 //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(144));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 114, 106, hug, 0, 0, 114, 106, RGB(255, 255, 255));
					}
					else if (quemeojogador == 9) {
						TransparentBlt(hdc, 450 - 30, 330, 88, 114, hug, 0, 0, 88, 114, RGB(255, 255, 255));
						Sleep(148);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 150, 150, auxdc, 0, 0, SRCCOPY);//pintar com chão

																				   //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(149));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 92, 122, hug, 0, 0, 92, 122, RGB(255, 255, 255));

					}
					else if (quemeojogador == 10) {
						TransparentBlt(hdc, 450 - 30, 330, 77, 99, hug, 0, 0, 77, 99, RGB(255, 255, 255));
						Sleep(148);//esperar um pouco
						BitBlt(hdc, 450 - 30, 330, 77, 99, auxdc, 0, 0, SRCCOPY);//pintar com chão

					//repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(154));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 66, 115, hug, 0, 0, 66, 115, RGB(255, 255, 255));
					}
					irparacima = 0; //repor

				}
				else if (ataque == 1) {//jogador ataca

					if (quemeojogador == 6) {
						m = LoadBitmap(hInstance, MAKEINTRESOURCE(135));
					}
					else if (quemeojogador == 7) {
						m = LoadBitmap(hInstance, MAKEINTRESOURCE(141));
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
						TransparentBlt(hdc, 450, 330, 92, 85, hug, 0, 0, 92, 85, RGB(255, 255, 255));
						Sleep(192);//esperar um pouco
						BitBlt(hdc, 450, 330, 92, 85, auxdc, 0, 0, SRCCOPY);//pintar com chão, a imagem é maior
						ataque = 0;//voltar a por o ataque a 0

								   //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(132));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 60, 105, hug, 0, 0, 60, 105, RGB(255, 255, 255));
					}
					else if (quemeojogador == 7) {
						TransparentBlt(hdc, 450, 330, 115, 106, hug, 0, 0, 115, 106, RGB(255, 255, 255));
						Sleep(192);//esperar um pouco
						BitBlt(hdc, 450, 330, 115, 106, auxdc, 0, 0, SRCCOPY);//pintar com chão, a imagem é maior
						ataque = 0;//voltar a por o ataque a 0

								   //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(137));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 69, 111, hug, 0, 0, 69, 111, RGB(255, 255, 255));
					}
					else if (quemeojogador == 8) {
						TransparentBlt(hdc, 450, 330, 71, 107, hug, 0, 0, 71, 107, RGB(255, 255, 255));
						Sleep(192);//esperar um pouco
						BitBlt(hdc, 450, 330, 71, 107, auxdc, 0, 0, SRCCOPY);//pintar com chão, a imagem é maior
						ataque = 0;//voltar a por o ataque a 0

								   //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(144));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 114, 106, hug, 0, 0, 114, 106, RGB(255, 255, 255));
					}
					else if (quemeojogador == 9) {
						TransparentBlt(hdc, 450, 330, 132, 117, hug, 0, 0, 132, 117, RGB(255, 255, 255));
						Sleep(152);//esperar um pouco
						BitBlt(hdc, 450, 330, 150, 150, auxdc, 0, 0, SRCCOPY);//pintar com chão, a imagem é maior
						ataque = 0;//voltar a por o ataque a 0

								   //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(149));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 92, 122, hug, 0, 0, 92, 122, RGB(255, 255, 255));

					}
					else if (quemeojogador == 10) {
						TransparentBlt(hdc, 450, 330, 87, 107, hug, 0, 0, 87, 107, RGB(255, 255, 255));
						Sleep(192);//esperar um pouco
						BitBlt(hdc, 450, 330, 87, 107, auxdc, 0, 0, SRCCOPY);//pintar com chão, a imagem é maior
						ataque = 0;//voltar a por o ataque a 0

								   //repintar
						mp = LoadBitmap(hInstance, MAKEINTRESOURCE(154));
						auxdc = CreateCompatibleDC(hdc);
						SelectObject(auxdc, mp);
						TransparentBlt(hdc, 450, 330, 66, 115, hug, 0, 0, 66, 115, RGB(255, 255, 255));
					}

					//voltar a desenhar normal que é feito fora do ciclo - vai ser feito aqui agora
					//----------------------bitmaps para os jogadores numero fazer daqui
				}
				
				Sleep(12); //pequena pausa

				DeleteDC(hug);
				

				//fazer retangulo e cenas
				Rectangle(hdc, 1052, 0, 1360, 450);
				m = LoadBitmap(hInstance, MAKEINTRESOURCE(148));
				hug = CreateCompatibleDC(hdc);
				SelectObject(hug, m);
				TransparentBlt(hdc, 1053, 0, 83, 87, hug, 0, 0, 83, 87, RGB(255, 255, 255));
				int vidad = 10;


				DeleteDC(auxdc);
				DeleteDC(hdc);
				EndPaint(hWnd, &pt);

				//nomes e pontos e cenas
				hdctexto = GetDC(hWnd);
				TextOut(hdctexto, 1056, 150, TEXT("Batman: "), _tcslen(TEXT("Batman: ")));

				
				ReleaseDC(hWnd, hdctexto);
			}

		break;
		case WM_KEYDOWN:
			//Imprimir
			//hdc = GetDC(hwnd);
			//TextOut(hdc, 450, 330, TEXT("&"), _tcslen(TEXT("&")));

			//ydown += 10;
			//		ReleaseDC(hwnd, hdc);
			if (wParam == VK_UP) {
				//330 -= 55;
				irparacima = 1;
				_tcscpy_s(Comando, 256, (TEXT("move cima")));
				//Comando[_tcslen(Comando) - 1] = '\0';
				res.ID_Cliente = ID_Cliente;
				Envia(Comando);
			}
			if (wParam == VK_DOWN) {
				//330 += 55;
				irparabaixo = 1;
				_tcscpy_s(Comando, 256, (TEXT("move baixo")));
				//Comando[_tcslen(Comando) - 1] = '\0';
				res.ID_Cliente = ID_Cliente;
				Envia(Comando);
			}
			if (wParam == VK_RIGHT) {
				//450 += 75;
				irparaadireita = 1;
				_tcscpy_s(Comando, 256, (TEXT("move direita")));
				//Comando[_tcslen(Comando) - 1] = '\0';
				res.ID_Cliente = ID_Cliente;
				Envia(Comando);
			}
			if (wParam == VK_LEFT) {
				//450 -= 75;
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
				ataque = 1;
				_tcscpy_s(Comando, 256, (TEXT("pedra mao")));
				//Comando[_tcslen(Comando) - 1] = '\0';
				res.ID_Cliente = ID_Cliente;
				Envia(Comando);
			}

			InvalidateRect(hWnd, NULL, FALSE); //por o 1 a false para deixar de sintilar
			break;

// Restantes mensagens têm processamento default
	default:
		return(DefWindowProc(hWnd,messg,wParam,lParam));
		break;
	}
	return(0);
}


// ============================================================================
// FUNÇÂO DE PROCESSAMENTO DA DIALOG BOX
// Esta função deve retronar TRUE se processou a mensagem e FALSE se não a 
// processou (daí os "return 1" que terminam os diversos "case")
// ============================================================================
BOOL CALLBACK DialogAutenticacao(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{	
	TCHAR login[30], passwd[30];
	HWND item;
	switch (messg){
	case WM_INITDIALOG:
		SetDlgItemText(hWnd, IDC_EDIT1, TEXT("introduza aqui o seu login"));
		SetDlgItemText(hWnd, IDC_EDIT2, TEXT("introduza aqui a sua password"));
		SendDlgItemMessage(hWnd, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM) TEXT("José"));
		SendDlgItemMessage(hWnd, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)TEXT("João"));
		SendDlgItemMessage(hWnd, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)TEXT("Conceição"));
		item = GetDlgItem(hWnd, IDC_EDIT1);
		wParam = IDC_EDIT1 | EN_SETFOCUS;
		SendMessage(hWnd, WM_COMMAND, (WPARAM)wParam, (LPARAM)item);
		return 1;
	case WM_COMMAND: //Clicou num controlo
		switch (LOWORD(wParam)){ //Que controlo?
			case IDCANCEL:
				EndDialog(hWnd, 0);
				return 1;
			
			case IDC_EDIT1:
				if (HIWORD(wParam) == EN_SETFOCUS){
					item = GetDlgItem(hWnd, IDC_EDIT1);
					//SendMesFage(item, EM_SETSEL, (WPARAM)0, (LPARAM)_tcslen(TEXT("introduza aqui a sua password")));
				}
				return 1;
			case IDOK:
				//Buscar as 2 strings
				//Comparar com admin admin : em C++ ==, em C _tcscmp(str1,str2)
				//MessageBox: autenticado se sim, fecho a caixa 
				//Limpar as caixas de texto se não				
				GetDlgItemText(hWnd, IDC_EDIT1, login, 30);
				GetDlgItemText(hWnd, IDC_EDIT2, passwd, 30);				
				if (_tcscmp(login, passwd)==0) //Se forem iguais
					MessageBox(hWnd, TEXT("Aceite"), TEXT("Login"), MB_OK);
				else
					MessageBox(hWnd, TEXT("Não aceite"), TEXT("Login"), MB_OK);
				
				return 1;
			case IDC_LIST1:
				if (HIWORD(wParam) == LBN_DBLCLK){
					//Preencher caixa de texto com o texto selecionado da listbox
					int i = SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETCURSEL, 0, 0);
					TCHAR login_temp[30];
					SendDlgItemMessage(hWnd, IDC_LIST1, LB_GETTEXT, i, (LPARAM)login_temp);
					SetDlgItemText(hWnd, IDC_EDIT1, (LPCWSTR) login_temp);

				}
				return 1;
		}
	}
	return 0;

}

// ******************************************************************
// Função "ActualizaEditBox":
// Escreve na Edit Box o nome do cliente actualmente seleccionado na
// List Box
// ******************************************************************
void ActualizaEditBox(HWND hw)	{
	TCHAR str[TAM];
	int i;
	
	//Verifica que elemento (índice) está seleccionado
	i = (int) SendDlgItemMessage(hw, IDC_LIST1, LB_GETCURSEL, 0, 0);	
	//Ler a palavra na List Box correspondente ao índice
	SendDlgItemMessage(hw, IDC_LIST1, LB_GETTEXT, i, (LPARAM) str);	
	//Colocá-la na Edit Box
	SetDlgItemText(hw, IDC_EDIT1, str);								
}

int Autenticacao(HWND hnd, LPVOID param, TCHAR nom[25], TCHAR pa[25]) {

	HANDLE hpipelocal = (HANDLE)param;
	DWORD n, p;
	BOOL ret;
	int i = 0, retorn;
	TCHAR buf[256];
	TCHAR frase[256];
	

	//autenticação do utilizador - nome e pass 

	//do {
		//_tprintf(TEXT("[Autenticação - coloque o nome]: "));
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
			//_tprintf(TEXT("[Servidor] Essa conta já esta em uso! Entre com outra conta\n"));
			//MessageBox(hnd, TEXT("Intruduza uma conta valida mas isro é assim"), TEXT("Sem sucesso"), MB_OK);
			
		//}

		retorn = 1;
		return retorn;
	//} while (resp != 1);
}
