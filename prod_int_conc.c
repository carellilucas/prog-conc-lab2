#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h> 

//variaveis globais
//tamanho do vetor
long int n;
//vetor de elementos
float *vet1, *vet2;
//numero de threads
int nthreads;

//funcao executada pelas threads
//estrategia de divisao de tarefas: blocos de n/nthreads elementos
void *ProdutoInterno (void *tid) {
  long int id = (long int) tid; //identificador da thread
  int ini, fim, bloco; //auxiliares para divisao do vetor em blocos
  float prod_int_local=0, *ret; //produto interno local
  
  bloco = n/nthreads; //tamanho do bloco de dados de cada thread
  ini = id*bloco; //posicao inicial do vetor
  fim = ini + bloco; //posicao final do vetor
  if (id==(nthreads-1)) fim = n; //a ultima thread trata os elementos restantes no caso de divisao nao exata

  //soma os valores 
  for(int i=ini; i<fim; i++) {
        prod_int_local += vet1[i]*vet2[i];
  }

  //retorna o resultado da produto
  ret = (float*) malloc(sizeof(float));
  if (ret!=NULL) *ret = prod_int_local;
  else printf("--ERRO: malloc() thread\n");
  pthread_exit((void*) ret);
}

//funcao principal do programa
int main(int argc, char *argv[]) {
  FILE *arq; //arquivo de entrada
  size_t ret; //retorno da funcao de leitura no arquivo de entrada
  double produto_ori; //produto registrado no arquivo
  float produto_par_global; //resultado do produto concorrente
  float var_relativa; //variacao relativa entre o produto interno original e o concorrente
  float *produto_retorno_threads; //auxiliar para retorno das threads

  pthread_t *tid_sistema; //vetor de identificadores das threads no sistema

  //valida e recebe os valores de entrada
  if(argc < 3) { printf("Use: %s <arquivo de entrada> <numero de threads> \n", argv[0]); exit(-1); }

  //abre o arquivo de entrada com os valores para serem somados
  arq = fopen(argv[1], "rb");
  if(arq==NULL) { printf("--ERRO: fopen()\n"); exit(-1); }

  //le o tamanho do vetor (primeira linha do arquivo)
  ret = fread(&n, sizeof(long int), 1, arq);
  if(!ret) {
     fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
     return 3;
  }

  //aloca espaco de memoria e carrega o vetor de entrada
  vet1 = (float*) malloc (sizeof(float) * n);
  if(vet1==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }
  ret = fread(vet1, sizeof(float), n, arq);
  if(ret < n) {
     fprintf(stderr, "Erro de leitura dos elementos do vetor 1\n");
     return 4;
  }
  vet2 = (float*) malloc (sizeof(float) * n);
  if(vet2==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }
  ret = fread(vet2, sizeof(float), n, arq);
  if(ret < n) {
     fprintf(stderr, "Erro de leitura dos elementos do vetor 2\n");
     return 4;
  }

  //le o numero de threads da entrada do usuario 
  nthreads = atoi(argv[2]);
  //limita o numero de threads ao tamanho do vetor
  if(nthreads>n) nthreads = n;

  //aloca espaco para o vetor de identificadores das threads no sistema
  tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
  if(tid_sistema==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }

  //cria as threads
  for(long int i=0; i<nthreads; i++) {
    if (pthread_create(&tid_sistema[i], NULL, ProdutoInterno, (void*) i)) {
       printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }

  //espera todas as threads terminarem e calcula a produto total das threads
  produto_par_global=0;
  for(int i=0; i<nthreads; i++) {
     if (pthread_join(tid_sistema[i], (void *) &produto_retorno_threads)) {
        printf("--ERRO: pthread_join()\n"); exit(-1);
     }
     produto_par_global += *produto_retorno_threads;
     free(produto_retorno_threads);
  }

  //imprime os resultados
  printf("\n");
  printf("produto_concorrente             = %.26f\n", produto_par_global);
  //le o produto interno registrado no arquivo
  ret = fread(&produto_ori, sizeof(double), 1, arq); 
  printf("\nproduto-ori                   = %.26lf\n", produto_ori);
  var_relativa = abs((produto_par_global - produto_ori)/produto_ori);
  printf("\nvariacao-relativa             = %.26lf\n", var_relativa);

  //desaloca os espacos de memoria
  free(vet1);
  free(vet2);
  free(tid_sistema);
  //fecha o arquivo
  fclose(arq);
  return 0;
}