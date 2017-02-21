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
	int j, random, i = 0;
	while (i < N) {
		auto t1 = high_resolution_clock::now();
		auto random = duration_cast<nanoseconds>(t1.time_since_epoch()).count();
		srand(random);
		if ((rand() % 100) < 70) {
			srand(0);
			for (j = 0; j < 10; j++) {
				mI[i].op = rand() % 2;
				mI[i].endBloco = rand() % TAMRAM;
				mI[i].endPalavra = rand() % 4;
				i++;
				if (i == N) break;
			}
		}
		else {
			srand(10);
			for (j = 0; j < 10; j++) {
				mI[i].op = rand() % 2;
				mI[i].endBloco = rand() % TAMRAM;
				mI[i].endPalavra = rand() % 4;
				i++;
				if (i == N) break;
			}
		}
	}
	mI[N - 1].op = 4;
	cout << "Memoria Instrucoes Montada" << endl;
}

int montarMemoriasInst2() {
#define TAM_FOR 100
#define PROP_FOR 70 //Probabiidade de ocorrência do laço de repetição, um numero entre 1 e 100;

	long int i, j, k, aleatorio;
	srand(time(NULL));

	//Arquivo onde será salvo as instruções;
	FILE *arquivo = fopen("programa.txt", "w");
	if (arquivo == NULL) {
		cout << "Erro ao abrir arquivo!" << endl;
		return 0;
	}

	//N é o numero de instruções;
	int n = 2;
	//instruc é o vetor que conta quantos parametros tem cada instrução;
	int instruc[] = { 1 , 1 };//Instruções numeradas de 0 a N-1;

											 //Gerando o laço de repetição do programa;
	vector< vector <long int> > For;
	for (i = 0; i < TAM_FOR; i++) {
		vector <long int> aux;
		aleatorio = rand() % n;
		aux.push_back(aleatorio);
		for (j = 0; j < instruc[aleatorio]; j++)
			aux.push_back(rand() % TAMRAM);
		For.push_back(aux);
	}

	for (i = 0; i < N;) {
		aleatorio = rand() % 100 + 1;
		if (aleatorio <= PROP_FOR) {
			for (j = 0; j < TAM_FOR && i < N; j++, i++) {
				fprintf(arquivo, "%ld", For[j][0]);
				for (k = 1; k <= instruc[For[j][0]]; k++)
					fprintf(arquivo, " %ld", For[j][k]);
				fprintf(arquivo, " %ld", rand() % 4);
				fprintf(arquivo, "\n");
			}
		}
		else {
			i++;
			aleatorio = rand() % n;
			fprintf(arquivo, "%ld", aleatorio);
			for (k = 0; k < instruc[aleatorio]; k++)
				fprintf(arquivo, " %ld", rand() % TAMRAM);
			fprintf(arquivo, " %ld", rand() % 4);
			fprintf(arquivo, "\n");
		}
	}

	fclose(arquivo);
	return 0;
}

int lerArquivo() {
	FILE *arquivo = fopen("programa.txt", "rb");
	if (arquivo == NULL) {
		cout << "Erro ao abrir arquivo!" << endl;
		return 0;
	}
	for (int k = 0; k < N; k++) {

		fscanf(arquivo, "%d", &mI[k].op);

		fscanf(arquivo, "%d", &mI[k].endBloco);

		fscanf(arquivo, "%d", &mI[k].endPalavra);

	}
	fclose(arquivo);
	mI[N - 1].op = 4;
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
			passaL2L1(endb, i, endp);
			return;
		}
	}
	cacheMissl2++;
	verificaL3(endb, endp);
}

void verificaL3(int endb, int endp) {
	for (int i = 0; i < TAML3; i++) {
		if (cacheNivell3[i].endBloco == endb) {
			cacheHitl3++;
			passaL3L2(endb, i, endp);
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
	trocaPosicaoCache(cacheNivell3, cacheNivell2, lugarCopiaL3, localMenosAcessadoL2);
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
	trocaPosicaoCache(cacheNivell2, cacheNivell1, lugarCopiaL2, localMenosAcessadoL1);
}

void estatisticas() {
	cout << endl;
	cout << "H3: " << cacheHitl3;
	cout << " M3: " << cacheMissl3 << endl;
	cout << "---------------------" << endl;
	cout << "H2: " << cacheHitl2;
	cout << " M2: " << cacheMissl2 << endl;
	cout << "---------------------" << endl;
	cout << "H1: " << cacheHitl1;
	cout << " M1: " << cacheMissl1 << endl;
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
	//cout << numero << ":" << decode << endl;
}

void opcode1(int endb, int endp) {
	int pos = 0;
	Palavra aux = verificaL1(endb, endp);
	int numero = aux.numero;
	int decode = aux.decode;
	int i = 0;
	if (decode != 0) {
		//cout << numero << ":" << decode << endl;
	}
	if (decode == 0) {
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

void trocaPosicaoCache(LinhaCache* cache1, LinhaCache* cache2, int pos1, int pos2) {
	Palavra palavraAux;
	bool boolAux;
	int intAux;
	for (int i = 0; i < 4; i++) {
		palavraAux = cache1[pos1].palavra[i];
		cache1[pos1].palavra[i] = cache2[pos2].palavra[i];
		cache2[pos2].palavra[i] = palavraAux;
	}// trocou as palavras
	// troca alterado
	boolAux = cache1[pos1].alterado;
	cache1[pos1].alterado = cache2[pos2].alterado;
	cache2[pos2].alterado = boolAux;
	// troca acesso
	intAux = cache1[pos1].acesso;
	cache1[pos1].acesso = cache2[pos2].acesso;
	cache2[pos2].acesso = intAux;
	//troca end de Bloco
	intAux = cache1[pos1].endBloco;
	cache1[pos1].endBloco = cache2[pos2].endBloco;
	cache2[pos2].endBloco = intAux;
}

int main() {
	montarMemoriasInst2();
	montarMemoriaDados();
	iniciarMemorias();
	lerArquivo();
	lerUpCodes();
	system("pause");
}