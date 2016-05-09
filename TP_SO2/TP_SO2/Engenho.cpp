#include "Engenho.h"

Engenho::Engenho(){

	jogoCriado = false;
	jogoIniciado = false;

}

bool Engenho::getJogoCriado() {
	return jogoCriado;
}

bool Engenho::getJogoIniciado() {
	return jogoIniciado;
}


int Engenho::ExecutaComando(string tcomando, string comando) {

	if (tcomando == "criarjogo") {
		jogoCriado = true;
		return 1;
	}
	else if (tcomando == "jogar") {
		if (jogoCriado == true) {
			jogoIniciado == true;
			return 2;
		}
		return 0;
	}
	else if (tcomando == "logout") {
		return 7;
	}
	else return 0;
}

void Engenho::NovoRegisto(LPCWSTR nome, LPCWSTR pass) {
	HKEY chave;
	DWORD queAconteceu, versao, tamanho;
	TCHAR autor[200];
	TCHAR keyName[200] = REGISTRY_KEY;
	wcscat_s(keyName, nome);


	//Criar/abrir uma chave em HKEY_CURRENT_USER\Software\MinhaAplicacao
	if (RegCreateKeyEx(HKEY_CURRENT_USER, keyName, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &chave, &queAconteceu) != ERROR_SUCCESS) {
		tcout << TEXT("Erro ao criar/abrir chave (%d)\n"), GetLastError();
	}
	else if (queAconteceu == REG_CREATED_NEW_KEY) {
		// fazer novo registo de um novo cliente, depois da chave ja estar a aberta

		RegSetValueEx(chave, TEXT("Nome"), 0, REG_SZ, (LPBYTE)nome, _tcslen(nome)*sizeof(TCHAR));

		RegSetValueEx(chave, TEXT("Pass"), 0, REG_SZ, (LPBYTE)pass, _tcslen(pass)*sizeof(TCHAR));
	}


}

bool Engenho::VerificaRegisto(LPCWSTR nome) {

	HKEY chave;
	DWORD queAconteceu, versao, tamanho;
	TCHAR autor[200];
	TCHAR keyName[200] = REGISTRY_KEY;

	wcscat_s(keyName, nome);

	//Criar/abrir uma chave em HKEY_CURRENT_USER\Software\MinhaAplicacao
	if (RegOpenKeyEx(HKEY_CURRENT_USER, keyName, 0, KEY_ALL_ACCESS, &chave) == ERROR_SUCCESS)
		return true;
	else return false;

}
