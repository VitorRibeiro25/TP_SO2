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

#define MAXCLIENTES 5

#define PIPENOME TEXT("\\\\.\\pipe\\teste")
#define PIPENOME2 TEXT("\\\\.\\pipe\\teste2")


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
	int mapa[20][20];
};


struct resposta res;
static int ID_Cliente = 0;
TCHAR Comando[256];
TCHAR nome[25];
TCHAR pass[25];
DWORD n;

// ============================================================================
// resource.h
// File criada pelo VC++ com resources necessárias ao
// programa (Ícones, Menus, Teclas de atalho e Dialog Boxes, neste exemplo)
// ============================================================================
#include "resource.h"
#define TAM 255
// Variável global hInstance usada para guardar "hInst" inicializada na função
// WinMain(). "hInstance" é necessária no bloco WinProc() para lançar a Dialog
// box em DialogBox(...) 
HINSTANCE hInstance;

typedef int (*PonteiroFuncao)(TCHAR*, TCHAR*);

// Declaração antecipada da função de processamento da janela "WndProc()"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Declaração antecipada da função de processamento da DialogBox, "DialogProc()"
BOOL CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DialogAutenticacao(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogUtilizadores(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);

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
struct fichas lista[NUM] = {
	{TEXT("Pedro Manuel Vieira"), 10133450, 40000},
	{TEXT("Antonio de Jesus"), 10345300, 3000},
	{TEXT("Maria Amelia"), 10456320, 35000},
	{TEXT("Jose Pedro Fonseca"), 10256700, 10000},
	{TEXT("Rosa Pereira"), 10434560, 65000},
	{TEXT("Ana Cristina Pires"), 10667800, 120000},
	{TEXT("Joao Peste"), 10945560, 2500}
};

TCHAR logins[NUM][30] = { TEXT("Pedro Manuel Vieira"), TEXT("António de Jesus"),
TEXT("Maria Amélia"), TEXT("José Pedro Fonseca"), TEXT("Rosa Pereira"), TEXT("Ana Cristina Pires"),
TEXT("João Peste") };

// ============================================================================
// WinMain()
// NOTA: Ver declaração de HACCEL, LoadAccelerators() e TranslateAccelerator()
//		 Estas são as modificações necessárias para tratar as teclas de atalho
//		 para opções do menu
// ============================================================================

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) 
	{
	HWND hWnd;			// handler da janela (a gerar por CreateWindow())
	MSG lpMsg;			// Estrutura das mensagens
	WNDCLASSEX wcApp;	// Estrutura que define a classe da janela
	HACCEL hAccel;		// Handler da resource accelerators (teclas de atalho
						// do menu)
	wcApp.cbSize = sizeof(WNDCLASSEX);	
	wcApp.hInstance = hInst;			
										
	wcApp.lpszClassName = szProgName;	
	wcApp.lpfnWndProc = WndProc;		
	wcApp.style = CS_HREDRAW | CS_VREDRAW ;
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
	hWnd = CreateWindow(
		szProgName,				// Nome da janela e/ou programa
		TEXT("Menus / Resources"),	// Título da janela
		WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME,	// Estilo da janela 
		0,			// Posição x 
		0,			// Posição y 
		520,			// Largura 
		550,			// Altura 
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

	while (GetMessage(&lpMsg,NULL,0,0)) {	
		//if(!TranslateAccelerator(hWnd, hAccel, &lpMsg)){
			TranslateMessage(&lpMsg);		// Pré-processamento da mensagem
			DispatchMessage(&lpMsg);		// Enviar a mensagem traduzida de volta
		//}									// ao Windows
	}
	return((int)lpMsg.wParam);	// Status = Parâmetro "wParam" da estrutura "lpMsg"
}

BOOL CALLBACK DialogNova(HWND hWnd, UINT messg, WPARAM wp, LPARAM lParam)
{
	switch (messg){
	case WM_COMMAND:
		if (LOWORD(wp) == IDC_BUTTON1) {
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
BOOL CALLBACK Trata(HWND hWnd, UINT messg, WPARAM w, LPARAM lParam){//Retorna FALSE se não tratar evento, TRUE se tratar o evento
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
				
					passa=Autenticacao(hWnd, hPipe2, nome, pass);
			if(passa==1) {
				EndDialog(hWnd, 1);
				return TRUE;
						}

			}
		}

		if (LOWORD(w) == IDCANCEL) {
			EndDialog(hWnd, 1);
			return TRUE;
		}

		break;
	case WM_INITDIALOG:

		break;
	case WM_CLOSE:
		EndDialog(hWnd, 1);
		return FALSE;
		break;
	default:
		break;
	}
	return FALSE;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) 
{
	int resposta;					// Resposta a MessageBox
	HWND h1, h2;
	TCHAR nome2[25];
	static HMENU hMenu = NULL;
	BOOL RES;
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
			
			RES=DialogBox(hInstance, (LPCWSTR) MAKEINTRESOURCE(111), hWnd, (DLGPROC)Trata);
			//h1=CreateDialog(hInstance, (LPCWSTR) IDD_DIALOG4, hWnd, (DLGPROC) Trata);
			//h2=CreateDialog(hInstance, (LPCWSTR)IDD_DIALOG5, hWnd, (DLGPROC)Trata);
			//ShowWindow(h1, SW_SHOW);
			//ShowWindow(h2, SW_SHOW);
			if (RES == TRUE) {
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

			break;
		case ID_Pontuacao:

			break;
		case ID_Ajudalogin:

			break;
		case ID_SAIRlogin:
			hMenu = LoadMenu(NULL, IDR_MENU2);
			SetMenu(hWnd, hMenu);
			break;

	 }

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
BOOL CALLBACK DialogProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) 
{
	int i;				// Index da List Box
	TCHAR str[TAM];		// Buffer para strings

	switch(messg) 
	{
// ************************************************************
// Inicialização da Dialog Box
// WM_INITDIALOG ocorre quando a Dialog Box é instanciada:
// Inicializar a Dialog Box preenchendo a List Box com os nomes
// dos clientes, seleccionar o primeiro e colocá-lo na Edit Box
// ************************************************************
		case WM_INITDIALOG:
/*  Preencher List Box
	LONG SendDlgItemMessage(
		HWND hDlg,      // handle of dialog box
		int nIDDlgItem, // identifier of control
		UINT Msg,       // message to send
		WPARAM wParam,  // first message parameter
		LPARAM lParam   // second message parameter	
Mensagens comuns:	
		LB_ADDSTRING	Adicionar a string apontada por lParam à LB (wParam não é usado)
		LB_GETCURSEL	Retorna o index do item seleccionado
		LB_SETCURSEL	Seleccionar o item de index wParam
		LB_FINDSTRING	Encontrar a string lParam a partir do index	wParam
		LB_SELECTSTRING	Igual a "findstring" mas selecciona o item se o encontrar
		LB_GETTEXT		Obter texto em lParam o item de index wParam				   
*/
			for (i=0; i<NUM; i++)
				SendDlgItemMessage(hWnd, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM) lista[i].nome);

			// Seleccionar o item 0 da List Box
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_SETCURSEL, 0, 0);

/* Mostrar item 0 da LB na Edit Box	
	BOOL SetDlgItemText(
		HWND hDlg,         // handle of dialog box
		int nIDDlgItem,    // identifier of control
		LPCTSTR lpString   // text to set	
*/
// Como a ListBox está ordenada alfabeticamente, é preciso ler o primeiro
// nome que nela figura, e que pode não coincidir com o primeiro elemento
// da lista "lista[0].nome"
			ActualizaEditBox(hWnd);
			SetDlgItemText(hWnd, IDC_EDIT1, TEXT("inserir login ççç"));
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)TEXT("Olá"));
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)TEXT("Açúcar"));
			return 1;

// ************************************************************
// Comandos provenientes dos controlos que a Dialog Box contém:
// wParam 
//		The high-order word specifies the notification code if the message is from a control. 
//		If the message is from an accelerator, this value is 1. 
//		If the message is from a menu, this value is zero. 
//		The low-order word specifies the identifier of the menu item, control, or accelerator. 
// lParam 
//		Handle to the control sending the message if the message is from a control. 
// 		Otherwise, this parameter is NULL. 
// ************************************************************
		case WM_COMMAND:
			switch(LOWORD(wParam)) 
			{
// ************************************************************
// Duplo click na List Box ou Botão Seleccionar: Seleccionar e mostrar na Edit Box
				
				case IDC_LIST1:
					if (HIWORD(wParam)==LBN_DBLCLK)	//Clique Duplo
						ConfirmarItemSeleccionado(hWnd);
					
					ActualizaEditBox(hWnd);
					break;

				case IDSELECCIONAR:
					ConfirmarItemSeleccionado(hWnd);
					return 1;
// ************************************************************
// Clicou no OK: Processo análogo ao anterior
				case IDOK:
					SetDlgItemText(hWnd, IDOK, TEXT("Premido"));
					ConfirmarItemSeleccionado(hWnd);
					return 1;
// ************************************************************
// Clicou Procurar: Procurar o texto da Edit Box na List Box
				case IDPROCURAR:
/* Obter texto da Edit Box (o texto fica em "str")
	UINT GetDlgItemText(
		HWND hDlg,       // handle of dialog box
		int nIDDlgItem,  // identifier of control
		LPTSTR lpString, // address of buffer for text
		int nMaxCount    // maximum size of string			
*/
					GetDlgItemText(hWnd, IDC_EDIT1, str, 80);
					// Procurar o texto "str" na List Box e se encontrar selecciona-o
					i = (int) SendDlgItemMessage(hWnd, IDC_LIST1, LB_SELECTSTRING, 0, (LPARAM) str);

					if (i!=LB_ERR) 
						//Se encontrou actualizar edit box com o nome completo
						ActualizaEditBox(hWnd);
									
					else
						// Se não encontrou, mostrar mensagem "não encontrado"						
						MessageBox(hWnd, str,TEXT("Registo Inexistente"), MB_OK);
					
					return 1;
// ***********************************************************
// Clicou no Close 
				case IDCCLOSE:
					EndDialog(hWnd, 0);
					return 1;
			}
		}	
	return 0;
}

// ******************************************************************
// Função "ConfirmarItemSeleccionado":
// Mostrar numa dialog box clicnte escolhido na ListBox e confirmá-lo
// Se sim, mostrar nome numa Edit Box
// ******************************************************************
void ConfirmarItemSeleccionado(HWND hw) {
	int i;				// Index da ListBox
	int res;			// Resultados (comparações, MessageBox ou outros)
	TCHAR str[TAM];		// Buffer de mensagens

	// Obter o indice da ficha seleccionada na LB 
	i = (int) SendDlgItemMessage(hw, IDC_LIST1, LB_GETCURSEL, 0, 0);

	// Mostrar uma caixa de mensagem com a ficha e confirmar 
// NOTA: Como a lista está por ordem alfabética (diferente da da lista
//		 a partir da qual foi preenchida) é preciso procurar na lista
//		 o nome actualmente seleccionado na List Box
	SendDlgItemMessage(hw, IDC_LIST1, LB_GETTEXT, i, (LPARAM) str);
	res=1;
	i=0;
	while (res) {
		res=_tcscmp(str, lista[i].nome);		// Devolve 0 quando strings iguais
		i++;
	}
	_stprintf_s(str, TAM / sizeof(TCHAR),  TEXT("NOME:\t%s\nBI:\t%u\nSALDO:\t%ld"), 
	        lista[i-1].nome, lista[i-1].bi, lista[i-1].saldo);
	MessageBox(hw, str, TEXT("Dados do Cliente"), MB_OK|MB_ICONQUESTION);
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

BOOL CALLBACK DialogUtilizadores(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam){
	int i;
//	HWND hImageCtl;
	switch (messg){
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return 1;

	case WM_INITDIALOG:
		for (i = 0; i < NUM; i++){
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)logins[i]);
		}
		/*hImageCtl = GetDlgItem(hWnd, ID_FOTO);
		SendMessage(hWnd, STM_SETIMAGE, ID_FOTO, (LPARAM)hImage);*/
		return 1;
	}
	return 0;
}

int Autenticacao(HWND hnd, LPVOID param, TCHAR nom[25], TCHAR pa[25]) {

	HANDLE hpipelocal = (HANDLE)param;
	DWORD n;
	BOOL ret;
	int i = 0, resp;
	TCHAR buf[256];
	TCHAR frase[256];

	//autenticação do utilizador - nome e pass 

	//do {
		//_tprintf(TEXT("[Autenticação - coloque o nome]: "));
		//fflush(stdin);
		//nom[_tcslen(nom) - 1] = '\0';
		wcscpy_s(Comando, 25, nom);
		//_fgetts(Comando, 256, nom);
		//Comando[_tcslen(Comando) - 1] = '\0';
		wcscpy_s(nome, 25, Comando);
		res.ID_Cliente = ID_Cliente;
		WriteFile(hPipe, Comando, _tcslen(Comando) * sizeof(TCHAR), &n, NULL);

		//pa[_tcslen(pa) - 1] = '\0';
		wcscpy_s(Comando, 25, pa);
		//_fgetts(Comando, 256, pa);
		//Comando[_tcslen(Comando) - 1] = '\0';
		wcscpy_s(pass, 25, Comando);
		res.ID_Cliente = ID_Cliente;
		WriteFile(hPipe, Comando, _tcslen(Comando) * sizeof(TCHAR), &n, NULL);

		ReadFile(hpipelocal, &resp, sizeof(resp), &n, NULL);
		if (resp == 0) {
			//_tprintf(TEXT("[Servidor] Essa conta já esta em uso! Entre com outra conta\n"));
			MessageBox(hnd, TEXT("Intruduza uma conta valida"), TEXT("Sem sucesso"), MB_OK);
			
		}
		return resp;
	//} while (resp != 1);
}
