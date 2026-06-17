#include <stdio.h>
int tam_mem_fisica;
int tam_pag;
int tam_max_processo;
int qnt_quadros;
int id = 0;

typedef struct {
    int id;
    int tamanho_bytes;
    int qtd_paginas;
    int *tabela_paginas;
    char *memoria_logica;
} Processo;


void *init_configs() {
    printf("Selecione o tamanho da memoria fisica:\n");
    scanf("%d", &tam_mem_fisica);
    printf("Digite o tamanho da pagina/quadro:\n");
    scanf("%d", &tam_pag);
    printf("Digite o tamanho maximo de um processo:\n");
    scanf("%d", &tam_max_processo);
    qnt_quadros = (tam_mem_fisica / tam_pag);
}

// Criar processo
Processo cria_processo(){
    Processo processo;

    processo.id = id;
    id += 1;
    printf("Digite o tamanho de bytes do processo");
    scanf("%d", &processo.tamanho_bytes);
    processo.qtd_paginas = (processo.tamanho_bytes + tam_pag - 1) / tam_pag;
}

// Visualizar memoria

// Visualizar tabela de páginas
int main(){

    
    init_configs();

    int opcao;

    printf("Gerenciador de memória\n");
    printf("Escolha uma opção: \n1- Criar Processo \n2- Visualizar Memória \n3- Visualizar tabela de páginas\n");
    scanf("%d", &opcao);

    if (opcao == 1)
    {
        printf("Criar processo foi selecionado\n");
    }
    else if (opcao == 2)
    {
        printf("Visualizar memoria\n");
    }
    else if (opcao == 3)
    {
        printf("Visualizar tabela de páginas\n");
    }
    else 
    {
        printf("escolha uma opção de 1 a 3\n");
    };

};