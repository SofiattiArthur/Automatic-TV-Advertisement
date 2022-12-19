/**
 * @file interface.h
 * @brief Operaçoes de interface com o usuario: recebimento de parametros, geracao de relatorio, comunicaçao de erros
 */
#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <confuse.h>
#include "matching.h"

#define CONF_PARAM          "-c"            /**<parametro que indica presenca do arquivo de configuracao*/
#define PPARAM              "-p"            /**<parametro que indica presenca do trexo de programacao anterior*/
#define EXT_LIST_DEF        ".txt"          /**<extencao dos arquivos do tipo lista (valor default)*/
#define EXT_MFCC_DEF        ".mfc"          /**<extencao dos arquivos do tipo MFCC (valor default)*/
#define EXT_DEST_DEF        ".csv"          /**<extencao dos arquivos do tipo saida (valor default)*/
#define MAXN                100             /**<tamanho maximo das strings*/
#define ERR_EXT             0x01            /**<Codigo de erro de extencao do arquivo*/
#define ERR_OPN             0x02            /**<Codigo de erro de abertura de arquivo*/
#define ERR_PAR             0x03            /**<Codigo de erro de parametro invalido*/
#define ERR_ENT             0xFF            /**<Codigo de erro de entrada invalida*/
#define TIME_DIF_DEF        2000            /**<Tempo de dessincronizacao aceitavel (em ms)*/
#define ERR_SIL             3               /**<Codigo de erro de excesso de silencio*/
#define ERR_MTC             9               /**<Codifo de erro de comparacao de coeficientes*/
#define SEM_ANTERIOR        3               /**<Numero minimo de argumentos de argv da funcao comparacao (-2)*/
#define COM_ANTERIOR        5               /**<Numero de argumentos de argv da funcao comparacao quando informado o corte anterior ao alvo (-2)*/
#define MXOCUR              500             /**<Numero maximo de ocorrencias*/
#define CONFNAME            "matching.conf" /**<Nome do arquivo de configuracao*/
#define NSA_ANT             3               /**<"Nao se aplica" flag referente ao corte anterior*/
#define ERRO_ANT            2               /**<"Nao foi possivel procurar" flag referente ao corte anterior*/
#define FIND_ANT            1               /**<"Encontrado" flag referente ao corte anterior*/
#define N_FIND_ANT          0               /**<"Nao encontrado" flag referente ao corte anterior*/

#define SAIDA_PADRAO        stderr          /**<Arquivo de saida padrao*/
#define NEI                 4               /**<Numero de argumentos do vetor de erros resevado para inicializacao*/

extern char EXT_LIST[10];   /**<Extensao do arquivo de lista de propagandas*/
extern char EXT_MFCC[10];   /**<Extensao dos arquivos de coeficientes MFCC*/
extern char EXT_DEST[10];   /**<Extensao do arquivo de saida*/
extern int TIME_DIF;        /**<Dessincronizacao tolerada no inicio da propaganda*/

int testa_inteiro(int valor, int li, int ls);

int testa_conf();

int new_conf(char* name);

int load_conf(char* name);

int conf_par(int argc, char** argv);

void desloca_argv(int argc, char **argv, int p);

void tolow(char* str);

char* stristr(char* str, char* ext);

int testa_extensao(char *str, char *ext);

unsigned char testa_arquivo(FILE** arq, char* name, char* ext);

unsigned char interpIn(int argc, char** argv, FILE** prop, FILE** prog, FILE** pprog, FILE** out);

int MAGIC(FILE* fil);

int err2(unsigned char inType, char* erros);

int errmt(int n, char*erros);

void RelGen(FILE* out, char* prop, char* prog, int* t, int* vr, unsigned char* flag, unsigned char* prev, unsigned char* g, unsigned int n);

int myfgets(char* fname, unsigned int s, FILE* fil);

void cpyMFCC(int16_t** in, int16_t** out, unsigned int s, unsigned int nCoef);

char testa_parametros(cabecalho C1, cabecalho C2);

int err_param(char* erros);

int16_t **loadFile(FILE* fil, cabecalho *C);

int loadAnt(cabecalho *Cpg, cabecalho Cg, int16_t*** prog, FILE *pprogf, char *erros, int *flagE);

int comparacao(int argc, char** argv, char *erros);

#endif // INTERFACE_H