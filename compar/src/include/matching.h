/**
 * @file matching.h
 * @brief Operaçoes de analise macroscópica das informaçoes geradas na comparaçao entre propaganda e programação em cada ponto
 */

#ifndef MATCHING2_H
#define MATCHING2_H

#include "coef_dist.h"

#define MINJ_DEF        3000                /**<Numero minimo de janelas em ms (valor default)*/
#define P               243                 /**<Percentual de relevancia da primeira coluna da MFCC (95%)*/
#define JAN             7                   /**<Tamanho da janela*/
#define DL              5                   /**<Multiplicador do limiar*/
#define VR_LIM_DEF      500                 /**<Valor minimo de VR (valor default)*/
#define LIM_SINC_DEF    256                 /**<Dessincronizacao maxima no final das propagandas em milissegundos (valor default)*/
#define ERRO_G          2                   /**<"Erro" flag relativa ao final da propaganda*/
#define FIND_G          1                   /**<"Encontrado" flag relativa ao final da propaganda*/
#define N_FIND_G        0                   /**<"Nao encontrado" flag relativa ao final da propaganda*/

extern int MINJ;        /**<Numero minimo de janelas em milissegundos*/
extern int VR_LIM;      /**<Precisao minima para incluir a ocorrencia no relatorio*/
extern int LIM_SINC;    /**<Dessincronizacao maxima no final da propaganda*/

int find_end(int16_t **prop, unsigned int sp);

int32_t* invers(int32_t* vec, unsigned int s);

int32_t* diff(int32_t* vec, unsigned int s);

int32_t limiar(int32_t *vec, unsigned int s);

int32_t maximus(int32_t* vec, unsigned int s, unsigned int* k);

int32_t minimus(int32_t* vec, unsigned int s, unsigned int* k);

int tempo(int k, unsigned int s);

int ocurr(int32_t** CD, unsigned int scd, unsigned int i, int32_t limiar1, int32_t limiar2, int* vr);

int CDtrat(int32_t** CD, unsigned int sp, unsigned int sg, unsigned int minJ);

int matching(int16_t** prop, int16_t** prog, unsigned int sp, unsigned int sg, int* t, int* vr, char flag, unsigned char* g);

#endif // MATCHING2_H
