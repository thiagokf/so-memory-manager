/* ============================================================================
 *  memoria_fisica.c
 *
 *  Implementação do gerenciador de memória física com MAPA DE BITS.
 *  Ver memoria_fisica.h para a documentação da interface pública.
 * ========================================================================== */

#include "memoria_fisica.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ----------------------------------------------------------------------------
 *  Macros auxiliares para mexer em bits individuais do mapa de bits.
 *  O bit do quadro 'i' fica no byte (i/8), na posição (i%8) dentro do byte.
 * -------------------------------------------------------------------------- */
#define BIT_GET(bm, i)   ( ((bm)[(i) >> 3] >> ((i) & 7)) & 1u )
#define BIT_SET(bm, i)   ( (bm)[(i) >> 3] |=  (unsigned char)(1u << ((i) & 7)) )
#define BIT_CLEAR(bm, i) ( (bm)[(i) >> 3] &= (unsigned char)~(1u << ((i) & 7)) )

/* Verifica se 'n' é potência de dois (n > 0 e só 1 bit ligado). */
static int eh_potencia_de_dois(int n) {
    return (n > 0) && ((n & (n - 1)) == 0);
}

/* ----------------------------------------------------------------------------
 *  Ciclo de vida
 * -------------------------------------------------------------------------- */

int mf_init(MemoriaFisica *mf, int tam_memoria, int tam_quadro) {
    int bytes_bitmap;

    if (mf == NULL) {
        return 1;
    }

    /* Validações exigidas pelo enunciado: potências de 2 e divisibilidade. */
    if (!eh_potencia_de_dois(tam_memoria) || !eh_potencia_de_dois(tam_quadro)) {
        fprintf(stderr,
                "[memoria_fisica] tamanhos devem ser potências de 2 "
                "(memoria=%d, quadro=%d)\n", tam_memoria, tam_quadro);
        return 2;
    }
    if (tam_quadro > tam_memoria || (tam_memoria % tam_quadro) != 0) {
        fprintf(stderr,
                "[memoria_fisica] memória física deve ser múltiplo do quadro\n");
        return 3;
    }

    mf->tam_memoria = tam_memoria;
    mf->tam_quadro  = tam_quadro;
    mf->num_quadros = tam_memoria / tam_quadro;
    mf->livres      = mf->num_quadros;

    /* A memória física em si: um vetor de bytes, inicializado com zeros. */
    mf->memoria = (unsigned char *) calloc((size_t) tam_memoria, 1);
    if (mf->memoria == NULL) {
        return 4;
    }

    /* Mapa de bits: precisamos de 1 bit por quadro -> arredonda pra cima. */
    bytes_bitmap = (mf->num_quadros + 7) / 8;
    mf->bitmap = (unsigned char *) calloc((size_t) bytes_bitmap, 1);
    if (mf->bitmap == NULL) {
        free(mf->memoria);
        mf->memoria = NULL;
        return 5;
    }

    /* calloc ja zerou tudo -> todos os quadros nascem LIVRES (bit 0). */
    return 0;
}

void mf_destroy(MemoriaFisica *mf) {
    if (mf == NULL) {
        return;
    }
    free(mf->memoria);
    free(mf->bitmap);
    mf->memoria = NULL;
    mf->bitmap = NULL;
    mf->tam_memoria = 0;
    mf->tam_quadro = 0;
    mf->num_quadros = 0;
    mf->livres = 0;
}

/* ----------------------------------------------------------------------------
 *  Gerência de quadros
 * -------------------------------------------------------------------------- */

int mf_quadros_livres(const MemoriaFisica *mf) {
    return (mf != NULL) ? mf->livres : 0;
}

double mf_percentual_livre(const MemoriaFisica *mf) {
    if (mf == NULL || mf->num_quadros == 0) {
        return 0.0;
    }
    return (100.0 * (double) mf->livres) / (double) mf->num_quadros;
}

int mf_quadro_ocupado(const MemoriaFisica *mf, int quadro) {
    if (mf == NULL || quadro < 0 || quadro >= mf->num_quadros) {
        return 0;
    }
    return (int) BIT_GET(mf->bitmap, quadro);
}

int mf_alocar_quadro(MemoriaFisica *mf) {
    int i;

    if (mf == NULL || mf->livres == 0) {
        return MF_SEM_QUADRO;
    }

    /* First-fit: varre o bitmap procurando o primeiro bit 0 (quadro livre). */
    for (i = 0; i < mf->num_quadros; i++) {
        if (BIT_GET(mf->bitmap, i) == 0u) {
            BIT_SET(mf->bitmap, i);
            mf->livres--;
            return i;
        }
    }

    /* Em teoria não chega aqui (livres > 0), mas fica como rede de segurança. */
    return MF_SEM_QUADRO;
}

int mf_liberar_quadro(MemoriaFisica *mf, int quadro) {
    if (mf == NULL || quadro < 0 || quadro >= mf->num_quadros) {
        return 1;
    }
    if (BIT_GET(mf->bitmap, quadro) == 1u) {
        BIT_CLEAR(mf->bitmap, quadro);
        mf->livres++;
    }
    return 0;
}

int mf_alocar_varios(MemoriaFisica *mf, int n, int *quadros_out) {
    int i, alocados = 0;

    if (mf == NULL || quadros_out == NULL || n < 0) {
        return 1;
    }
    if (n == 0) {
        return 0; /* nada a fazer */
    }

    /* Checagem antecipada: se nem há 'n' quadros livres, nem começa a alocar.
     * Isto é o tratamento de "não há memória suficiente para o processo". */
    if (n > mf->livres) {
        return 2;
    }

    /* Aloca um a um. Se por algum motivo faltar no meio, desfaz tudo. */
    for (i = 0; i < n; i++) {
        int q = mf_alocar_quadro(mf);
        if (q == MF_SEM_QUADRO) {
            /* rollback: libera os que ja tinham sido pegos nesta chamada */
            int j;
            for (j = 0; j < alocados; j++) {
                mf_liberar_quadro(mf, quadros_out[j]);
            }
            return 2;
        }
        quadros_out[alocados++] = q;
    }
    return 0;
}

int mf_liberar_varios(MemoriaFisica *mf, int n, const int *quadros) {
    int i;
    if (mf == NULL || quadros == NULL || n < 0) {
        return 1;
    }
    for (i = 0; i < n; i++) {
        mf_liberar_quadro(mf, quadros[i]);
    }
    return 0;
}

/* ----------------------------------------------------------------------------
 *  Acesso aos bytes da memória
 * -------------------------------------------------------------------------- */

int mf_escrever_byte(MemoriaFisica *mf, int endereco, unsigned char valor) {
    if (mf == NULL || endereco < 0 || endereco >= mf->tam_memoria) {
        return 1;
    }
    mf->memoria[endereco] = valor;
    return 0;
}

int mf_ler_byte(const MemoriaFisica *mf, int endereco, unsigned char *valor) {
    if (mf == NULL || valor == NULL || endereco < 0 || endereco >= mf->tam_memoria) {
        return 1;
    }
    *valor = mf->memoria[endereco];
    return 0;
}

/* ----------------------------------------------------------------------------
 *  Visualização
 * -------------------------------------------------------------------------- */

void mf_visualizar(const MemoriaFisica *mf) {
    int q, b, base;

    if (mf == NULL) {
        return;
    }

    printf("\n========== ESTADO DA MEMÓRIA FÍSICA ==========\n");
    printf("Tamanho total : %d bytes\n", mf->tam_memoria);
    printf("Tamanho quadro: %d bytes\n", mf->tam_quadro);
    printf("Quadros       : %d (livres: %d)\n", mf->num_quadros, mf->livres);
    printf("Memória livre : %.1f%%\n", mf_percentual_livre(mf));
    printf("----------------------------------------------\n");

    for (q = 0; q < mf->num_quadros; q++) {
        base = q * mf->tam_quadro;
        printf("Quadro %3d [%s] :", q,
               BIT_GET(mf->bitmap, q) ? "OCUPADO" : " LIVRE ");
        /* Mostra o conteúdo (em hexadecimal) de cada byte do quadro. */
        for (b = 0; b < mf->tam_quadro; b++) {
            printf(" %02X", mf->memoria[base + b]);
        }
        printf("\n");
    }
    printf("==============================================\n\n");
}
