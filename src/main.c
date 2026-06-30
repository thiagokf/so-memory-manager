#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

static int eh_potencia_de_dois(int n) {
    return (n > 0) && ((n & (n - 1)) == 0);
}

static int ler_valor_configuracao(const char *nome, int padrao, int *valor) {
    char buffer[64];

    printf("%s [%d]: ", nome, padrao);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        *valor = padrao;
        return 0;
    }

    buffer[strcspn(buffer, "\r\n")] = '\0';
    if (buffer[0] == '\0') {
        *valor = padrao;
        return 0;
    }

    if (sscanf(buffer, "%d", valor) != 1) {
        printf("Entrada inválida para %s.\n", nome);
        return 1;
    }

    return 0;
}

static void configurar_inicial(int *tam_memoria, int *tam_pagina, int *tam_max_processo) {
    int valido = 0;

    while (!valido) {
        printf("=== Configuração inicial ===\n");
        printf("Informe os valores abaixo. Pressione Enter para usar o valor padrão.\n");

        if (ler_valor_configuracao("Tamanho da memória física (bytes)", TAM_MEMORIA_FISICA, tam_memoria) != 0) {
            continue;
        }
        if (ler_valor_configuracao("Tamanho da página/quadro (bytes)", TAM_PAGINA, tam_pagina) != 0) {
            continue;
        }
        if (ler_valor_configuracao("Tamanho máximo de um processo (bytes)", TAM_MAX_PROCESSO, tam_max_processo) != 0) {
            continue;
        }

        if (!eh_potencia_de_dois(*tam_memoria) || !eh_potencia_de_dois(*tam_pagina) || !eh_potencia_de_dois(*tam_max_processo)) {
            printf("Erro: os valores devem ser potências de 2.\n");
            continue;
        }
        if (*tam_memoria <= 0 || *tam_pagina <= 0 || *tam_max_processo <= 0) {
            printf("Erro: os valores devem ser positivos.\n");
            continue;
        }
        if (*tam_memoria % *tam_pagina != 0) {
            printf("Erro: a memória física deve ser múltipla do tamanho da página.\n");
            continue;
        }
        if (*tam_max_processo % *tam_pagina != 0) {
            printf("Erro: o tamanho máximo do processo deve ser múltiplo do tamanho da página.\n");
            continue;
        }
        if (*tam_max_processo > *tam_memoria) {
            printf("Erro: o tamanho máximo do processo não pode ser maior que a memória física.\n");
            continue;
        }
        if (*tam_pagina > *tam_memoria) {
            printf("Erro: o tamanho da página não pode ser maior que a memória física.\n");
            continue;
        }

        valido = 1;
    }
}

static void menu_visualizar_memoria(const MemoriaFisica *mf) {
    mf_visualizar(mf);
}

static void menu_criar_processo(MemoriaFisica *mf, int tam_pagina, int tam_max_processo) {
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

    printf("Digite o tamanho do processo em bytes (máximo %d): ", tam_max_processo);
    if (scanf("%d", &tamanho) != 1) {
        printf("Entrada inválida.\n");
        while (getchar() != '\n');
        return;
    }

    if (tamanho <= 0 || tamanho > tam_max_processo) {
        printf("Erro: tamanho inválido. Deve ser entre 1 e %d bytes.\n", tam_max_processo);
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

    if (proc_criar(&processos[slot], id, tamanho, mf, tam_pagina, tam_max_processo) == 0) {
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

static void menu_matar_processo(MemoriaFisica *mf) {
    int id;
    Processo *p;

    printf("Digite o ID do processo a ser encerrado: ");
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

    /* Destrói o processo e libera seus quadros */
    proc_destruir(p, mf);
    if (num_processos > 0) num_processos--;
    printf("Processo %d removido da memória.\n", id);
}

int main(void) {
    MemoriaFisica mf;
    int opcao;
    int tam_memoria_fisica = TAM_MEMORIA_FISICA;
    int tam_pagina = TAM_PAGINA;
    int tam_max_processo = TAM_MAX_PROCESSO;

    srand((unsigned) time(NULL));

    configurar_inicial(&tam_memoria_fisica, &tam_pagina, &tam_max_processo);

    /* Inicializa a memória física com os valores configurados no início do programa */
    if (mf_init(&mf, tam_memoria_fisica, tam_pagina) != 0) {
        fprintf(stderr, "Falha ao inicializar a memória física.\n");
        return 1;
    }

    printf("\n=== Simulador de Gerenciamento de Memória com Paginação ===\n");
    printf("Memória física: %d bytes | Página/Quadro: %d bytes | Max processo: %d bytes\n",
           tam_memoria_fisica, tam_pagina, tam_max_processo);

    do {
        printf("\n--- Menu ---\n");
        printf("1 - Visualizar memória\n");
        printf("2 - Criar processo\n");
        printf("3 - Visualizar tabela de páginas\n");
        printf("4 - Matar um processo\n");
        printf("0 - Sair\n");
        printf("Opção: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (opcao) {
            case 1: menu_visualizar_memoria(&mf); break;
            case 2: menu_criar_processo(&mf, tam_pagina, tam_max_processo); break;
            case 3: menu_visualizar_tabela();      break;
            case 4: menu_matar_processo(&mf);        break;
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
