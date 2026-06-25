#ifndef PROCESSO_H
#define PROCESSO_H

#include "memoria_fisica.h"

/* Número máximo de processos simultâneos no simulador. */
#define MAX_PROCESSOS 64

typedef struct {
    int id;
    int tamanho;          /* tamanho em bytes da memória lógica */
    int qtd_paginas;
    unsigned char *mem_logica;   /* vetor de bytes da memória lógica */
    int *tabela_paginas;         /* tabela de páginas: índice = página, valor = quadro */
    int ativo;                   /* 1 se o processo existe, 0 se livre */
} Processo;

/*
 * Cria um processo: aloca memória lógica com valores aleatórios,
 * aloca quadros na memória física e copia as páginas.
 * Preenche a tabela de páginas com o mapeamento página -> quadro.
 *
 * Retorna 0 em sucesso, != 0 em erro (tamanho inválido, sem memória, id duplicado).
 */
int proc_criar(Processo *proc, int id, int tamanho, MemoriaFisica *mf, int tam_pagina, int tam_max);

/*
 * Destrói um processo: libera os quadros na memória física e a memória dinâmica.
 */
void proc_destruir(Processo *proc, MemoriaFisica *mf);

/*
 * Exibe a tabela de páginas do processo.
 */
void proc_visualizar_tabela(const Processo *proc);

#endif /* PROCESSO_H */
