# Simulador de Gerenciamento de Memoria com Paginacao

Trabalho da disciplina de Sistemas Operacionais (Unidade 2) — UFSC.

Simula o gerenciamento de memoria usando paginacao: memoria fisica representada por vetor de bytes, controle de quadros livres com mapa de bits, criacao de processos com alocacao nao contigua e tabela de paginas.

---

## Dependencias

- **GCC** (ou outro compilador C compativel com C99)
- **Make** (GNU Make)

No macOS ambos vem com o Xcode Command Line Tools:

```bash
xcode-select --install
```

No Ubuntu/Debian:

```bash
sudo apt install build-essential
```

---

## Estrutura do projeto

```
include/
  config.h             -> parametros configuraveis (tamanho da memoria, pagina, max processo)
  memoria_fisica.h     -> interface do modulo de memoria fisica
  processo.h           -> interface do modulo de processos
src/
  main.c               -> menu interativo (ponto de entrada)
  memoria_fisica.c     -> implementacao da memoria fisica com mapa de bits
  processo.c           -> implementacao de processos e tabela de paginas
Makefile               -> automacao de compilacao
README.md              -> este arquivo
```

---

## Como compilar

```bash
make
```

Isso gera o executavel `simulador`.

### Outros comandos do Makefile

| Comando      | O que faz                                             |
|--------------|-------------------------------------------------------|
| `make`       | Compila todos os `.c` e gera o executavel `simulador` |
| `make clean` | Remove os arquivos objeto (`.o`) e o executavel       |

Para recompilar do zero:

```bash
make clean && make
```

---

## Como executar

```bash
./simulador
```

Ao iniciar, o programa exibe a configuracao atual e apresenta o menu:

```
=== Simulador de Gerenciamento de Memoria com Paginacao ===
Memoria fisica: 128 bytes | Pagina/Quadro: 8 bytes | Max processo: 64 bytes

--- Menu ---
1 - Visualizar memoria
2 - Criar processo
3 - Visualizar tabela de paginas
0 - Sair
Opcao:
```

---

## Opcoes do menu

### 1 - Visualizar memoria

Exibe o estado completo da memoria fisica:
- Percentual de memoria livre
- Cada quadro com seu status (LIVRE ou OCUPADO) e conteudo em hexadecimal

Exemplo de saida:

```
========== ESTADO DA MEMORIA FISICA ==========
Tamanho total : 128 bytes
Tamanho quadro: 8 bytes
Quadros       : 16 (livres: 14)
Memoria livre : 87.5%
----------------------------------------------
Quadro   0 [OCUPADO] : 7B 8F CC F3 6B D3 03 CE
Quadro   1 [OCUPADO] : F1 D5 69 53 93 CA 80 E6
Quadro   2 [ LIVRE ] : 00 00 00 00 00 00 00 00
...
==============================================
```

**Como interpretar:** cada linha mostra um quadro da memoria fisica. `OCUPADO` significa que esta alocado a algum processo. Os valores hexadecimais sao os bytes armazenados naquele quadro (conteudo da pagina do processo).

### 2 - Criar processo

Solicita:
- **ID do processo**: um numero inteiro que identifica o processo (deve ser unico)
- **Tamanho em bytes**: quantidade de memoria que o processo precisa

O que acontece internamente:
1. Valida que o tamanho nao excede o maximo configurado
2. Calcula quantas paginas o processo precisa (arredondando para cima)
3. Aloca os quadros necessarios na memoria fisica (tudo-ou-nada)
4. Preenche a memoria logica do processo com valores aleatorios
5. Copia cada pagina logica para o quadro fisico correspondente
6. Cria a tabela de paginas com o mapeamento pagina -> quadro

Mensagens de erro possiveis:
- `"tamanho invalido"` — tamanho maior que o maximo ou menor/igual a zero
- `"memoria fisica insuficiente"` — nao ha quadros livres suficientes
- `"ja existe um processo com ID X"` — ID duplicado

### 3 - Visualizar tabela de paginas

Solicita o ID do processo e exibe sua tabela de paginas:

```
===== TABELA DE PAGINAS - Processo 1 =====
Tamanho do processo: 20 bytes
Numero de paginas  : 3
-------------------------------------------
  Pagina  ->  Quadro
       0  ->       0
       1  ->       1
       2  ->       2
===========================================
```

**Como interpretar:** cada linha da tabela mostra o mapeamento de uma pagina logica para um quadro fisico. Por exemplo, `Pagina 0 -> Quadro 0` significa que a primeira pagina do processo esta armazenada no quadro 0 da memoria fisica. Os quadros nao precisam ser contiguos — essa e a essencia da paginacao.

### 0 - Sair

Encerra o simulador, liberando toda a memoria alocada.

---

## Configuracao

Os parametros do simulador ficam em `config.h`. Todos devem ser **potencias de 2**:

```c
#define TAM_MEMORIA_FISICA 128   /* tamanho total da memoria fisica em bytes */
#define TAM_PAGINA           8   /* tamanho de uma pagina/quadro em bytes    */
#define TAM_MAX_PROCESSO    64   /* tamanho maximo de um processo em bytes   */
```

Para alterar, edite os valores em `config.h` e recompile com `make clean && make`. O arquivo possui verificacoes em tempo de compilacao que impedem valores invalidos (nao potencia de 2, memoria nao multipla da pagina, etc.).

---

## Exemplo de uso completo

```
$ ./simulador
=== Simulador de Gerenciamento de Memoria com Paginacao ===
Memoria fisica: 128 bytes | Pagina/Quadro: 8 bytes | Max processo: 64 bytes

--- Menu ---
1 - Visualizar memoria
2 - Criar processo
3 - Visualizar tabela de paginas
0 - Sair
Opcao: 2
Digite o ID do processo: 1
Digite o tamanho do processo em bytes (maximo 64): 20
Processo 1 criado com sucesso (20 bytes, 3 paginas).

--- Menu ---
Opcao: 2
Digite o ID do processo: 2
Digite o tamanho do processo em bytes (maximo 64): 30
Processo 2 criado com sucesso (30 bytes, 4 paginas).

--- Menu ---
Opcao: 1

========== ESTADO DA MEMORIA FISICA ==========
Tamanho total : 128 bytes
Tamanho quadro: 8 bytes
Quadros       : 16 (livres: 9)
Memoria livre : 56.2%
----------------------------------------------
Quadro   0 [OCUPADO] : DE 9D B2 B3 0B 6F 82 7A
Quadro   1 [OCUPADO] : 74 52 DE EB F6 50 D2 72
Quadro   2 [OCUPADO] : 06 18 1E E9 1C CF 9C 7B
Quadro   3 [OCUPADO] : 2D BB E0 07 18 C4 20 03
Quadro   4 [OCUPADO] : 62 1D E1 F4 20 DA 28 6F
Quadro   5 [OCUPADO] : 74 AA 96 65 AC D4 A6 E6
Quadro   6 [OCUPADO] : F3 80 9C 8E C5 DF B3 22
Quadro   7 [ LIVRE ] : 00 00 00 00 00 00 00 00
...
==============================================

--- Menu ---
Opcao: 3
Digite o ID do processo: 1

===== TABELA DE PAGINAS - Processo 1 =====
Tamanho do processo: 20 bytes
Numero de paginas  : 3
-------------------------------------------
  Pagina  ->  Quadro
       0  ->       0
       1  ->       1
       2  ->       2
===========================================

--- Menu ---
Opcao: 0
Encerrando.
```

---

## Detalhes tecnicos

### Mapa de bits

O controle de quadros livres/ocupados usa um **mapa de bits**: cada bit representa um quadro (0 = livre, 1 = ocupado). Isso gasta apenas 1 bit por quadro. As operacoes de bit usam deslocamento e mascara para acessar bits individuais.

### Alocacao tudo-ou-nada

Ao criar um processo, todos os quadros necessarios sao alocados de uma vez. Se nao houver quadros suficientes, nenhum e alocado (rollback automatico), evitando vazamento de memoria.

### Fragmentacao interna

Se o tamanho de um processo nao for multiplo exato da pagina, a ultima pagina fica parcialmente utilizada. Exemplo: processo de 20 bytes com pagina de 8 bytes usa 3 paginas (24 bytes alocados), desperdicando 4 bytes no ultimo quadro. Isso e inerente a paginacao.
