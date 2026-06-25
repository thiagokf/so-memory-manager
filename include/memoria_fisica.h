#ifndef MEMORIA_FISICA_H
#define MEMORIA_FISICA_H

/* ============================================================================
 *  memoria_fisica.h
 *
 *  Gerenciador de Memória Física para o simulador de paginação.
 *
 *  Responsável por:
 *    - representar a memória física como um vetor de bytes;
 *    - dividir essa memória em QUADROS (frames) de tamanho fixo;
 *    - controlar quais quadros estão livres/ocupados usando um MAPA DE BITS;
 *    - alocar e liberar quadros sob demanda;
 *    - calcular o percentual de memória livre;
 *    - exibir o estado da memória (opção "Visualizar memória" do menu).
 * ========================================================================== */

#include <stddef.h>

/* Código de retorno usado quando NÃO há quadro livre para alocar. */
#define MF_SEM_QUADRO (-1)

/*
 * Estrutura que representa toda a memória física do sistema.
 *
 *  memoria      -> vetor de bytes que é a memória física de verdade.
 *  bitmap       -> mapa de bits. Cada bit representa 1 quadro:
 *                    bit = 0  -> quadro LIVRE
 *                    bit = 1  -> quadro OCUPADO
 *                  Cada byte do bitmap guarda o estado de 8 quadros.
 *  tam_memoria  -> tamanho total da memória física, em bytes.
 *  tam_quadro   -> tamanho de cada quadro (= tamanho de uma página), em bytes.
 *  num_quadros  -> quantidade de quadros = tam_memoria / tam_quadro.
 *  livres       -> contador de quadros livres (atualizado a cada alloc/free,
 *                  evita ter que varrer o bitmap só para saber o percentual).
 */
typedef struct {
    unsigned char *memoria;
    unsigned char *bitmap;
    int tam_memoria;
    int tam_quadro;
    int num_quadros;
    int livres;
} MemoriaFisica;

/* ---------------------------------------------------------------------------
 *  Ciclo de vida
 * ------------------------------------------------------------------------- */

/*
 * Inicializa a memória física.
 *  - tam_memoria e tam_quadro devem ser potências de 2 (requisito do trabalho);
 *  - tam_memoria deve ser múltiplo de tam_quadro;
 *  - aloca os vetores internos e marca todos os quadros como livres.
 *
 *  Retorna 0 em sucesso, ou valor != 0 em caso de erro de parâmetro/memória.
 */
int mf_init(MemoriaFisica *mf, int tam_memoria, int tam_quadro);

/* Libera a memória dinâmica usada internamente pela estrutura. */
void mf_destroy(MemoriaFisica *mf);

/* ---------------------------------------------------------------------------
 *  Gerência de quadros (o núcleo da alocação não contígua)
 * ------------------------------------------------------------------------- */

/* Quantidade de quadros atualmente livres. */
int mf_quadros_livres(const MemoriaFisica *mf);

/* Percentual de memória livre (0.0 a 100.0). */
double mf_percentual_livre(const MemoriaFisica *mf);

/* Diz se um quadro específico está ocupado (1) ou livre (0). */
int mf_quadro_ocupado(const MemoriaFisica *mf, int quadro);

/*
 * Aloca o primeiro quadro livre que encontrar (estratégia "first-fit" sobre
 * o mapa de bits) e retorna o seu índice. Marca o quadro como ocupado.
 *
 *  Retorna o índice do quadro (0 .. num_quadros-1) em sucesso,
 *  ou MF_SEM_QUADRO se não houver quadro livre.
 */
int mf_alocar_quadro(MemoriaFisica *mf);

/*
 * Libera um quadro previamente alocado (marca como livre no bitmap).
 *  Retorna 0 em sucesso, != 0 se o índice for inválido.
 */
int mf_liberar_quadro(MemoriaFisica *mf, int quadro);

/*
 * Aloca um BLOCO de 'n' quadros de uma vez (tudo-ou-nada).
 *
 * Útil na criação de um processo: ou consegue TODOS os quadros que o processo
 * precisa, ou não altera nada. Se faltar espaço no meio do caminho, desfaz
 * (rollback) os quadros já alocados nesta chamada, evitando deixar memória
 * "vazada" presa a um processo que não chegou a ser criado.
 *
 *  - quadros_out deve apontar para um vetor com capacidade >= n; em caso de
 *    sucesso, recebe os índices dos n quadros alocados.
 *  - Retorna 0 em sucesso.
 *  - Retorna != 0 se não houver 'n' quadros livres (nada é alocado).
 */
int mf_alocar_varios(MemoriaFisica *mf, int n, int *quadros_out);

/*
 * Libera um bloco de 'n' quadros (ex.: ao destruir um processo).
 * 'quadros' aponta para os índices a liberar. Retorna 0 em sucesso.
 */
int mf_liberar_varios(MemoriaFisica *mf, int n, const int *quadros);

/* ---------------------------------------------------------------------------
 *  Acesso aos bytes da memória (usado para carregar páginas nos quadros)
 * ------------------------------------------------------------------------- */

/* Escreve 'valor' no endereço físico 'endereco'. Retorna 0 em sucesso. */
int mf_escrever_byte(MemoriaFisica *mf, int endereco, unsigned char valor);

/* Lê o byte no endereço físico 'endereco' (em 'valor'). Retorna 0 em sucesso. */
int mf_ler_byte(const MemoriaFisica *mf, int endereco, unsigned char *valor);

/* ---------------------------------------------------------------------------
 *  Visualização (opção "Visualizar memória" do menu)
 * ------------------------------------------------------------------------- */

/*
 * Exibe no terminal:
 *   - o percentual de memória livre;
 *   - cada quadro, indicando se está LIVRE ou OCUPADO, e o conteúdo dos bytes.
 */
void mf_visualizar(const MemoriaFisica *mf);

#endif /* MEMORIA_FISICA_H */
