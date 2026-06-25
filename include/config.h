#ifndef CONFIG_H
#define CONFIG_H

/* ============================================================================
 *  config.h
 *
 *  Parâmetros configuráveis do simulador de paginação, centralizados em um
 *  único lugar. TODOS os módulos do grupo (memória física, processo/tabela de
 *  páginas e interface) devem incluir este arquivo e usar estas constantes,
 *  para garantir que página e quadro tenham sempre o MESMO tamanho.
 *
 *  Regras (exigidas pelo enunciado):
 *    - todos os tamanhos são potências de 2;
 *    - TAM_MEMORIA_FISICA deve ser múltiplo de TAM_PAGINA;
 *    - TAM_MAX_PROCESSO   deve ser múltiplo de TAM_PAGINA;
 *    - página e quadro têm o mesmo tamanho (TAM_PAGINA).
 * ========================================================================== */

/* Tamanho total da memória física, em bytes. (256 / 8 = 32 quadros) */
#define TAM_MEMORIA_FISICA 256

/* Tamanho de uma página = tamanho de um quadro, em bytes. */
#define TAM_PAGINA 8

/* Tamanho máximo permitido para um processo (sua memória lógica), em bytes.
 * 64 / 8 = no máximo 8 páginas por processo. */
#define TAM_MAX_PROCESSO 64

/* ----------------------------------------------------------------------------
 *  Verificações automáticas em tempo de compilação.
 *  Se alguém trocar um valor por algo inválido, o programa nem compila e
 *  avisa o motivo (melhor do que descobrir o erro só na execução).
 * -------------------------------------------------------------------------- */

/* Potência de 2: um número potência de 2 tem (N & (N-1)) == 0. */
#if (TAM_MEMORIA_FISICA & (TAM_MEMORIA_FISICA - 1)) != 0
#  error "TAM_MEMORIA_FISICA precisa ser potência de 2"
#endif
#if (TAM_PAGINA & (TAM_PAGINA - 1)) != 0
#  error "TAM_PAGINA precisa ser potência de 2"
#endif
#if (TAM_MAX_PROCESSO & (TAM_MAX_PROCESSO - 1)) != 0
#  error "TAM_MAX_PROCESSO precisa ser potência de 2"
#endif

/* Divisibilidade. */
#if (TAM_MEMORIA_FISICA % TAM_PAGINA) != 0
#  error "TAM_MEMORIA_FISICA precisa ser múltiplo de TAM_PAGINA"
#endif
#if (TAM_MAX_PROCESSO % TAM_PAGINA) != 0
#  error "TAM_MAX_PROCESSO precisa ser múltiplo de TAM_PAGINA"
#endif

/* Número total de quadros na memória física (derivado, não editar). */
#define NUM_QUADROS (TAM_MEMORIA_FISICA / TAM_PAGINA)

#endif /* CONFIG_H */
