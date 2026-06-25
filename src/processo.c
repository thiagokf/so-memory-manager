#include "processo.h"

#include <stdio.h>
#include <stdlib.h>

int proc_criar(Processo *proc, int id, int tamanho, MemoriaFisica *mf, int tam_pagina, int tam_max) {
    int i, b;

    if (proc == NULL || mf == NULL) return 1;

    if (tamanho <= 0 || tamanho > tam_max) {
        fprintf(stderr, "Erro: tamanho %d inválido (máximo: %d bytes).\n", tamanho, tam_max);
        return 2;
    }

    proc->id = id;
    proc->tamanho = tamanho;
    proc->qtd_paginas = (tamanho + tam_pagina - 1) / tam_pagina;
    proc->ativo = 0;

    /* Aloca e inicializa a memória lógica com valores aleatórios */
    proc->mem_logica = (unsigned char *) malloc((size_t)(proc->qtd_paginas * tam_pagina));
    if (proc->mem_logica == NULL) return 3;

    for (i = 0; i < proc->qtd_paginas * tam_pagina; i++) {
        proc->mem_logica[i] = (unsigned char)(rand() % 256);
    }

    /* Aloca a tabela de páginas */
    proc->tabela_paginas = (int *) malloc((size_t) proc->qtd_paginas * sizeof(int));
    if (proc->tabela_paginas == NULL) {
        free(proc->mem_logica);
        proc->mem_logica = NULL;
        return 4;
    }

    /* Aloca quadros na memória física (tudo-ou-nada) */
    if (mf_alocar_varios(mf, proc->qtd_paginas, proc->tabela_paginas) != 0) {
        fprintf(stderr, "Erro: memória física insuficiente para o processo %d (%d quadros necessários, %d livres).\n",
                id, proc->qtd_paginas, mf_quadros_livres(mf));
        free(proc->mem_logica);
        free(proc->tabela_paginas);
        proc->mem_logica = NULL;
        proc->tabela_paginas = NULL;
        return 5;
    }

    /* Copia cada página lógica para o quadro físico correspondente */
    for (i = 0; i < proc->qtd_paginas; i++) {
        int quadro = proc->tabela_paginas[i];
        int base_logica = i * tam_pagina;
        int base_fisica = quadro * tam_pagina;
        for (b = 0; b < tam_pagina; b++) {
            mf_escrever_byte(mf, base_fisica + b, proc->mem_logica[base_logica + b]);
        }
    }

    proc->ativo = 1;
    return 0;
}

void proc_destruir(Processo *proc, MemoriaFisica *mf) {
    if (proc == NULL || !proc->ativo) return;

    if (mf != NULL && proc->tabela_paginas != NULL) {
        mf_liberar_varios(mf, proc->qtd_paginas, proc->tabela_paginas);
    }

    free(proc->mem_logica);
    free(proc->tabela_paginas);
    proc->mem_logica = NULL;
    proc->tabela_paginas = NULL;
    proc->ativo = 0;
}

void proc_visualizar_tabela(const Processo *proc) {
    int i;

    if (proc == NULL || !proc->ativo) {
        printf("Processo não encontrado ou inativo.\n");
        return;
    }

    printf("\n===== TABELA DE PAGINAS - Processo %d =====\n", proc->id);
    printf("Tamanho do processo: %d bytes\n", proc->tamanho);
    printf("Número de páginas  : %d\n", proc->qtd_paginas);
    printf("-------------------------------------------\n");
    printf("  Pagina  ->  Quadro\n");
    for (i = 0; i < proc->qtd_paginas; i++) {
        printf("  %6d  ->  %6d\n", i, proc->tabela_paginas[i]);
    }
    printf("===========================================\n\n");
}
