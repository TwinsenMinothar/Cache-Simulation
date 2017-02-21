#pragma once


#ifndef HEADER_H
#define HEADER_H

#define TAML1 8
#define TAML2 16
#define TAML3 128
#define N 100000
#define TAMRAM 1000

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <vector>


typedef struct p {
	int numero;
	int decode;
} Palavra;

typedef struct b {
	Palavra palavra[4];
} BlocoMP;

typedef struct l {
	Palavra palavra[4];
	bool alterado;
	int acesso;
	int endBloco;
} LinhaCache;

typedef struct i {
	int endBloco;
	int endPalavra;
	int op;
} Instrucao;

void lerUpCodes();
void montarMemoriaInst();
void montarMemoriaDados();
void iniciarMemorias();
Palavra verificaL1(int, int);
void verificaL2(int, int);
void verificaL3(int, int);
void passaL2L1(int, int, int);
void passaL3L2(int, int, int);
void passaRAML3(int, int);
void cacheTrocarValorL1(int, int, int);
void estatisticas();
void opcode0(int, int);
void opcode1(int, int);
void trocaPosicaoCache(LinhaCache* cache1, LinhaCache* cache2, int pos1, int pos2);
int montarMemoriasInst2();
int lerArquivo();


#endif HEADER_H