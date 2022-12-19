#ifndef FUNCOES_H
#define FUNCOES_H

#include <stdio.h>
#include <string.h>
#include <dirent.h>


//Retorna a posição do(s) caractere(s) na string, se não encontrar retorna 0
unsigned int testa_extensao(char *str, char *ext);

//Retorna 0 se o arquivo/diretório não existe e 1 se ele existe
unsigned char existe(char *str);

//Retorna o nome do arquivo
void nome_arquivo(char *arq, char *mfc);

//Modifica a extensão do arquivo, recebendo o nome do arquivo, posição da extensão(posicao do ponto) e extensão final(Ex.: ".txt")
void muda_extensao(char *arq, int pos, char *ext);

//Retorna a posição do caracter na string
char* stristr( char* str1, const char* str2 );

//Retorna 0 se valor está dentro do intervalo e 1 se não
unsigned char intervalo(int valor, int li, int ls);

#endif // FUNCOES_H
