/*
Programa auxiliar para gerar um vetor de floats 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1000 //valor maximo de um elemento do vetor
//descomentar o define abaixo caso deseje imprimir uma versao do vetor gerado no formato texto
#define TEXTO 

int main(int argc, char*argv[]) {
    float *vetor1, *vetor2; //vetores que serão gerados
    long int n; //qtde de elementos em cada vetor
    float elem1, elem2; //valores gerados para incluir nos vetores
    double prodInt=0; //soma total dos elementos gerados
    FILE * descritorArquivo; //descritor do arquivo de saida
    size_t ret; //retorno da funcao de escrita no arquivo de saida

    //recebe os argumentos de entrada
    if(argc < 3) {
        fprintf(stderr, "Digite: %s <dimensao> <nome arquivo saida>\n", argv[0]);
        return 1;
    }
    n = atoi(argv[1]);

    //aloca memoria para o vetor
    vetor1 = (float*) malloc(sizeof(float) * n);
    if(!vetor1) {
        fprintf(stderr, "Erro de alocao da memoria do vetor 1\n");
        return 2;
    }

    vetor2 = (float*) malloc(sizeof(float) * n);
    if(!vetor2) {
        fprintf(stderr, "Erro de alocao da memoria do vetor 2\n");
        return 2;
    }

    //preenche o vetor com valores float aleatorios
    srand(time(NULL));
    for(long int i=0; i<n; i++) {
        elem1 = (rand() % MAX/3.0) - 200; 
        elem2 = (rand() % MAX/3.0) - 200;
  
        vetor1[i] = elem1;
        vetor2[i] = elem2;
  
        prodInt += elem1 * elem2;
    }

    //imprimir na saida padrao os vetores gerados
    #ifdef TEXTO
    fprintf(stdout, "%ld\n", n);
    for(long int i=0; i<n; i++) {
        fprintf(stdout, "%f ",vetor1[i]);
    }
    fprintf(stdout, "\n");
    for(long int i=0; i<n; i++) {
        fprintf(stdout, "%f ",vetor2[i]);
    }
    fprintf(stdout, "\n");
    fprintf(stdout, "%lf\n", prodInt);
    #endif

    //escreve os vetores no arquivo
    //abre o arquivo para escrita binaria
    descritorArquivo = fopen(argv[2], "wb");
    if(!descritorArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 3;
    }
    // escreve dimensão
    fwrite(&n, sizeof(long int), 1, descritorArquivo);

    // escreve vetor1
    ret = fwrite(vetor1, sizeof(float), n, descritorArquivo);
    if(ret != n) {
        fprintf(stderr, "Erro de escrita no arquivo (vetor1)\n");
        return 4;
    }

    // escreve vetor2
    ret = fwrite(vetor2, sizeof(float), n, descritorArquivo);
    if(ret != n) {
        fprintf(stderr, "Erro de escrita no arquivo (vetor2)\n");
        return 5;
    }

    // escreve produto interno
    ret = fwrite(&prodInt, sizeof(double), 1, descritorArquivo);


    //finaliza o uso das variaveis
    fclose(descritorArquivo);
    free(vetor1);
    free(vetor2);
    return 0;
} 
