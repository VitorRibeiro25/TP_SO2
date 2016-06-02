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


int Engenho::ExecutaComando(string tcomando, string comando, Jogador *jog) {

	if (tcomando == "criarjogo") {
		if (jogoCriado == false) {
			if (comando == "pre") {
				jogoCriado = true;
				return 1;
			}
			if (comando == "ran") {
				jogoCriado = true;
				return 1;
			}
			return 0;
		}
		else return -1;
	}
	else if (tcomando == "jogar") {
		if (jogoCriado == true && jogoIniciado == false) {
			jogoIniciado = true;
			return 2;
		}
		else return -1;
	}

	else if (tcomando == "entrar" && comando == "jogo") {
		if (jogoCriado == true && jogoIniciado == true) {
			return 3;
		}
		else return -1;
	}
	else if (tcomando == "posicao" && comando == "atual") {
		if (jogoCriado == true && jogoIniciado == true)
			return 4;
		else return -1;
	}
	else if (tcomando == "move") {
		if (jogoCriado == true && jogoIniciado == true) {
			if (comando == "direita") {
				return 5;
			}
			else if (comando == "esquerda") {
				return 5;
			}
			else if (comando == "cima") {
				return 5;

			}
			else if (comando == "baixo") {
				return 5;
			}
			else return 0;
		}
		else return -1;
	}
	else if (tcomando == "pedra" && "mao") {

		if (jogoCriado == true && jogoIniciado == true) {
			return 6;
		}
		else return -1;

	}

	else if (tcomando == "logout") {
		return 7;
	}
	else return 0;
}

tstring Engenho::PosicaoJogador(Jogador *j) {

	tstringstream aux;


	aux << TEXT("[Servidor] O jogador encontra-se atualmente na posicao [x,y]: [") << j->getPosX() << "][" << j->getPosY() << "]" << "\n";


	return aux.str();

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
