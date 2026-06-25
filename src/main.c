#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "memoria_fisica.h"
#include "processo.h"

/* Vetor global de processos */
static Processo processos[MAX_PROCESSOS];
static int num_processos = 0;

/* Busca um processo pelo id. Retorna ponteiro ou NULL. */
static Processo *buscar_processo(int id) {
    int i;
    for (i = 0; i < num_processos; i++) {
        if (processos[i].ativo && processos[i].id == id) {
            return &processos[i];
        }
    }
    return NULL;
}

static void menu_visualizar_memoria(const MemoriaFisica *mf) {
    mf_visualizar(mf);
}

static void menu_criar_processo(MemoriaFisica *mf) {
    int id, tamanho;

    printf("Digite o ID do processo: ");
    if (scanf("%d", &id) != 1) {
        printf("Entrada inválida.\n");
        while (getchar() != '\n');
        return;
    }

    if (buscar_processo(id) != NULL) {
        printf("Erro: já existe um processo com ID %d.\n", id);
        return;
    }

    printf("Digite o tamanho do processo em bytes (máximo %d): ", TAM_MAX_PROCESSO);
    if (scanf("%d", &tamanho) != 1) {
        printf("Entrada inválida.\n");
        while (getchar() != '\n');
        return;
    }

    if (tamanho <= 0 || tamanho > TAM_MAX_PROCESSO) {
        printf("Erro: tamanho inválido. Deve ser entre 1 e %d bytes.\n", TAM_MAX_PROCESSO);
        return;
    }

    if (num_processos >= MAX_PROCESSOS) {
        printf("Erro: número máximo de processos atingido (%d).\n", MAX_PROCESSOS);
        return;
    }

    /* Encontra um slot livre no vetor */
    int slot = -1;
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        if (!processos[i].ativo) {
            slot = i;
            break;
        }
    }
    if (slot < 0) {
        printf("Erro: sem slot disponível.\n");
        return;
    }

    if (proc_criar(&processos[slot], id, tamanho, mf, TAM_PAGINA, TAM_MAX_PROCESSO) == 0) {
        num_processos++;
        printf("Processo %d criado com sucesso (%d bytes, %d páginas).\n",
               id, tamanho, processos[slot].qtd_paginas);
    }
}

static void menu_visualizar_tabela(void) {
    int id;
    Processo *p;

    printf("Digite o ID do processo: ");
    if (scanf("%d", &id) != 1) {
        printf("Entrada inválida.\n");
        while (getchar() != '\n');
        return;
    }

    p = buscar_processo(id);
    if (p == NULL) {
        printf("Processo %d não encontrado.\n", id);
        return;
    }

    proc_visualizar_tabela(p);
}

int main(void) {
    MemoriaFisica mf;
    int opcao;

    srand((unsigned) time(NULL));

    /* Inicializa a memória física com os valores definidos em config.h */
    if (mf_init(&mf, TAM_MEMORIA_FISICA, TAM_PAGINA) != 0) {
        fprintf(stderr, "Falha ao inicializar a memória física.\n");
        return 1;
    }

    printf("=== Simulador de Gerenciamento de Memória com Paginação ===\n");
    printf("Memória física: %d bytes | Página/Quadro: %d bytes | Max processo: %d bytes\n\n",
           TAM_MEMORIA_FISICA, TAM_PAGINA, TAM_MAX_PROCESSO);

    do {
        printf("--- Menu ---\n");
        printf("1 - Visualizar memória\n");
        printf("2 - Criar processo\n");
        printf("3 - Visualizar tabela de páginas\n");
        printf("0 - Sair\n");
        printf("Opção: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (opcao) {
            case 1: menu_visualizar_memoria(&mf); break;
            case 2: menu_criar_processo(&mf);     break;
            case 3: menu_visualizar_tabela();      break;
            case 0: printf("Encerrando.\n");       break;
            default: printf("Opção inválida.\n");   break;
        }
    } while (opcao != 0);

    /* Limpa todos os processos ativos */
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        if (processos[i].ativo) {
            proc_destruir(&processos[i], &mf);
        }
    }
    mf_destroy(&mf);

    return 0;
}
