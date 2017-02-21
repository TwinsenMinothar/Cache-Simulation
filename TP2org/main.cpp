#include "header.h"

using namespace std;
using namespace std::chrono;


int PC = 0;
int IR = -1; //valor opcode
int* MAR = NULL; //valor endere�o bloco ou linha 
int* MARp = NULL; //valor endereco palavra
Instrucao MBR;
string MQ;
int cacheMissl1, cacheHitl1;
int cacheMissl2, cacheHitl2;
int cacheMissl3, cacheHitl3;
int* tempo = new int[N];

LinhaCache *cacheNivell1 = new LinhaCache[TAML1];
LinhaCache *cacheNivell2 = new LinhaCache[TAML2];
LinhaCache *cacheNivell3 = new LinhaCache[TAML3];
BlocoMP *RAM = new BlocoMP[TAMRAM];
Instrucao *mI = new Instrucao[N];

void montarMemoriaInst() {
	int j, i = 0;
	while (i < N) {
		srand(i);
		if ((rand() % 100 + 1) < 70) {
			for (j = 0; j < 50; j++) {
				srand(j);
				mI[i].op = rand() % 2;
				mI[i].endBloco = rand() % 1000;
				mI[i].endPalavra = rand() % 4;
				i++;
				if (i == N) break;
			}
		}
		else {
			for (j = 0; j < 50; j++) {
				auto timeNow = high_resolution_clock::now();
				auto timeInt = duration_cast<nanoseconds>(timeNow.time_since_epoch()).count();
				srand(timeInt);
				mI[i].op = rand() % 2;
				mI[i].endBloco = rand() % 1000;
				mI[i].endPalavra = rand() % 4;
				i++;
				if (i == N) break;
			}
		}
	}
	mI[N - 1].op = 4;
	cout << "Memoria Instrucoes Montada" << endl;
}

void montarMemoriaDados() { // string palavras[4]
	srand(15);
	for (int i = 0; i < TAMRAM; i++) {
		for (int j = 0; j < 4; j++) {
			RAM[i].palavra[j].numero = (rand() % 300) + 1;
			RAM[i].palavra[j].decode = 0;
		}
	}
	cout << "Memoria Dados Montada" << endl;
}

void iniciarMemorias() {
	cacheHitl1, cacheHitl2, cacheHitl3, cacheMissl1, cacheMissl2, cacheMissl3 = 0;
	for (int i = 0; i < TAML1; i++) {
		cacheNivell1[i].acesso = 0;
		cacheNivell1[i].alterado = false;
		cacheNivell1[i].endBloco = -1;
	}
	for (int i = 0; i < TAML2; i++) {
		cacheNivell2[i].acesso = 0;
		cacheNivell2[i].alterado = false;
		cacheNivell2[i].endBloco = -1;
	}
	for (int i = 0; i < TAML3; i++) {
		cacheNivell3[i].acesso = 0;
		cacheNivell3[i].alterado = false;
		cacheNivell3[i].endBloco = -1;
	}
	for (int i = 0; i < N; i++) {
		tempo[i] = 0;
	}
	cout << "Memorias Iniciadas" << endl;
}

Palavra verificaL1(int endb, int endp) {
inicioL1:
	for (int i = 0; i < TAML1; i++) {
		if (cacheNivell1[i].endBloco == endb) {
			cacheNivell1[i].acesso++;
			cacheHitl1++;
			return cacheNivell1[i].palavra[endp];
		}
	}
	cacheMissl1++;
	cacheHitl1--;
	verificaL2(endb, endp);
	goto inicioL1;
}

void verificaL2(int endb, int endp) {
	for (int i = 0; i < TAML2; i++) {
		if (cacheNivell2[i].endBloco == endb) {
			cacheHitl2++;
			passaL2L1(endb, endp, i);
			return;
		}
	}
	cacheMissl2++;
	verificaL3(endb, endp);
}

void verificaL3(int endb, int endp) {
	for (int i = 0; i < TAML3; i++) {
		if (cacheNivell3[i].endBloco == endb) {
			passaL3L2(endb, endp, i);
			cacheHitl3++;
			return;
		}
	}
	cacheMissl3++;
	passaRAML3(endb, endp);
}

void passaRAML3(int endb, int endp) {
	int menosUsadoL3 = cacheNivell3[0].acesso;
	int localMenosAcessadoL3 = 0;
	for (int i = 0; i < TAML3; i++) {
		if (cacheNivell3[i].acesso < menosUsadoL3)
			localMenosAcessadoL3 = i;
		if (cacheNivell3[i].acesso == 0)
			break;
	}
	if (cacheNivell3[localMenosAcessadoL3].alterado == true) { // atualiza ram se nescessario
		for (int i = 0; i < 4; i++) {
			RAM[cacheNivell3[localMenosAcessadoL3].endBloco].palavra[i] = cacheNivell3[localMenosAcessadoL3].palavra[i];
		}
	}
	cacheNivell3[localMenosAcessadoL3].acesso = 0;
	cacheNivell3[localMenosAcessadoL3].alterado = false;
	cacheNivell3[localMenosAcessadoL3].endBloco = endb;
	for (int i = 0; i < 4; i++) {
		cacheNivell3[localMenosAcessadoL3].palavra[i] = RAM[endb].palavra[i];
	}
	passaL3L2(endb, localMenosAcessadoL3, endp);
}

void passaL3L2(int endb, int lugarCopiaL3, int endp) {
	int menosUsadoL2 = cacheNivell2[0].acesso;
	int localMenosAcessadoL2 = 0;
	for (int i = 0; i < TAML2; i++) {
		if (cacheNivell2[i].acesso < menosUsadoL2)
			localMenosAcessadoL2 = i;
		if (cacheNivell2[i].acesso == 0)
			break;
	}
	Palavra aux;
	bool auxb;
	int auxi;
	auxb = cacheNivell3[lugarCopiaL3].alterado;
	cacheNivell3[lugarCopiaL3].alterado = cacheNivell2[localMenosAcessadoL2].alterado;
	cacheNivell2[localMenosAcessadoL2].alterado = auxb;
	auxi = cacheNivell3[lugarCopiaL3].acesso;
	cacheNivell3[lugarCopiaL3].acesso = cacheNivell2[localMenosAcessadoL2].acesso;
	cacheNivell2[localMenosAcessadoL2].acesso = auxi; 
	cacheNivell3[lugarCopiaL3].endBloco = cacheNivell2[localMenosAcessadoL2].endBloco;
	cacheNivell2[localMenosAcessadoL2].endBloco = endb;
	for (int i = 0; i < 4; i++) {
		aux = cacheNivell2[localMenosAcessadoL2].palavra[i];
		cacheNivell2[localMenosAcessadoL2].palavra[i] = cacheNivell3[lugarCopiaL3].palavra[i];
		cacheNivell3[lugarCopiaL3].palavra[i] = aux;
	}
	passaL2L1(endb, localMenosAcessadoL2, endp);
}

void passaL2L1(int endb, int lugarCopiaL2, int endp) {
	int menosUsadoL1 = cacheNivell1[0].acesso;
	int localMenosAcessadoL1 = 0;
	for (int i = 0; i < TAML1; i++) {
		if (cacheNivell1[i].acesso < menosUsadoL1)
			localMenosAcessadoL1 = i;
		if (cacheNivell1[i].acesso == 0)
			break;
	}
	int auxi;
	Palavra aux;
	bool auxb;
	cacheNivell2[lugarCopiaL2].endBloco = cacheNivell1[localMenosAcessadoL1].endBloco;
	cacheNivell1[localMenosAcessadoL1].endBloco = endb;
	auxb = cacheNivell2[lugarCopiaL2].alterado;
	cacheNivell2[lugarCopiaL2].alterado = cacheNivell1[localMenosAcessadoL1].alterado;
	cacheNivell1[localMenosAcessadoL1].alterado = auxb;
	auxi = cacheNivell2[lugarCopiaL2].acesso;
	cacheNivell2[lugarCopiaL2].acesso = cacheNivell1[localMenosAcessadoL1].acesso;
	cacheNivell1[localMenosAcessadoL1].acesso = auxi;
	for (int i = 0; i < 4; i++) {
		aux = cacheNivell1[localMenosAcessadoL1].palavra[i];
		cacheNivell1[localMenosAcessadoL1].palavra[i] = cacheNivell2[lugarCopiaL2].palavra[i];
		cacheNivell2[lugarCopiaL2].palavra[i] = aux;
	}
}

void estatisticas() {
	cout << endl;
	cout << "H3   " << cacheHitl3;
	cout << " M3   " << cacheMissl3 << endl;
	cout << "---------------------" << endl;
	cout << "H2   " << cacheHitl2;
	cout << " M2   " << cacheMissl2 << endl;
	cout << "---------------------" << endl;
	cout << "H1   " << cacheHitl1;
	cout << " M1   " << cacheMissl1 << endl;
	cout << "---------------------" << endl;
}

void lerUpCodes() {
	int OP = mI[0].op;
	int PC = 0;
	int endBloco;
	int endPalavra;
	while (OP != 4) {
		OP = mI[PC].op;
		endBloco = mI[PC].endBloco;
		endPalavra = mI[PC].endPalavra;
		if (OP == 0)
			opcode0(endBloco, endPalavra);
		else if (OP == 1)
			opcode1(endBloco, endPalavra);
		else if (OP == 4) {
			estatisticas();
			return;
		}
		PC++;
	}
}

void opcode0(int endb, int endp) {
	Palavra palavra = verificaL1(endb, endp);
	int numero = palavra.numero;
	int decode = palavra.decode;
	cout << numero << ":" << decode << endl;
}

void opcode1(int endb, int endp) {
	int pos = 0;
	Palavra aux = verificaL1(endb, endp);
	int numero = aux.numero;
	int decode = aux.decode;
	int i = 0;
	if (decode != 0)
		cout << numero << ":" << decode << endl;
	else {
		while ((rand() % 300) + 1 != numero)
			i++;
		cacheTrocarValorL1(endb, endp, i);
	}
}

void cacheTrocarValorL1(int endb, int endp, int decode) {
	int pos = -1;
	for (int i = 0; i < TAML1; i++) {
		if (cacheNivell1[i].endBloco == endb) {
			pos = i;
			break;
		}
	}
	if (pos != -1) {
		cacheNivell1[pos].alterado = true;
		cacheNivell1[pos].palavra[endp].decode = decode;
	}
}

int main() {
	montarMemoriaDados();
	montarMemoriaInst();
	iniciarMemorias();
	lerUpCodes();
	system("pause");
}