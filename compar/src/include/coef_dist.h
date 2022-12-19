/**
 * @file coef_dist.h
 * @brief Operações referentes ao cálculo da distância entre propaganda e programação em cada ponto
 */
#ifndef COEF_DIST_H
#define COEF_DIST_H
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define LIP1				j			      /**<Limite inferior dos vetores de prop na primeira comparação*/
#define LSP1				sprop		    /**<Limite superior dos vetores de prop na primeira comparação*/
#define LIP2				0			      /**<Limite inferior dos vetores de prop na segunda comparação*/
#define LSP2				sprop		    /**<Limite superior dos vetores de prop na segunda comparação*/
#define LIP3				0			      /**<Limite inferior dos vetores de prop na terceira comparação*/
#define LSP3				j			      /**<Limite superior dos vetores de prop na terceira comparação*/

#define LIG1				0			      /**<Limite inferior dos vetores de prog na primeira comparação*/
#define LSG1				(sprop-j)   /**<Limite superior dos vetores de prog na primeira comparação*/
#define LIG2				j			      /**<Limite inferior dos vetores de prog na segunda comparação*/
#define LSG2				(sprop+j)	  /**<Limite superior dos vetores de prog na segunda comparação*/
#define LIG3				(sprog-j)	  /**<Limite inferior dos vetores de prog na terceira comparação*/
#define LSG3				sprog		    /**<Limite superior dos vetores de prog na terceira comparação*/

#define RELEVANCIA_DEF	    13		/**<Número de colunas consideradas no calculo da diferença de coeficientes (valor default)*/
#define LIMSIL			    (-3072)		/**<Valor mínimo de energia para que o vetor não seja considerado silêncio (-12*256)*/
#define VOLMIN_DEF			60			  /**<Porcentagem mínima de termos superiores ao valor de silencio para que o bloco seja considerado (valor default)*/

#define MIN(a,b)			(((a)<(b))?(a):(b))			/**<menor valor entre a e b*/
#define MAX(a,b)			(((a)>(b))?(a):(b))			/**<maior valor entre a e b*/

/**
 * Implementacao da funcao trapmf do Matlab \n
 *    se x>a, retorna 1 \n
 *    se x<b, retorna 0 \n
 *    se x<a && x>b, retorna (x-a)/(b-a)
 */
#define trapmf(x, a, b)		MAX(MIN((x-a)/(b-a),1),0)

extern int RELEVANCIA;  /**<Numero de coeficientes MFCC utilizados na comparacao*/
extern int VOLMIN;      /**<Porcentagem minima de audio significativo*/

/**
 * @brief Cabeçalho para arquivo binário ".mfc"
 */
typedef struct 
{
  u_int8_t mfcc[4];       /**< string "MFCC" marca o inicio do arquivo (4 bytes)*/
  u_int16_t n_fft;        /**< numero de pontos utilizados em cada janela para calcular a fft (2 bytes)*/
  u_int16_t step;         /**< tamanho do intervalo entre duas janelas (2 bytes)*/
  u_int16_t n_coef;       /**< numero de coeficientes gerados por janela (2 bytes)*/
  u_int16_t comp;         /**< numero de coeficientes salvos no arquivo por janela (2 bytes)*/
  u_int32_t data_size;    /**< numero de bytes ate o final do arquivo (4 bytes)*/
} cabecalho;

//Funções

int *diffI(unsigned int *x, unsigned int sx);

int16_t sum(int16_t *vec, unsigned int t);

int silencio(int16_t** vec, unsigned int s);

int16_t max(int16_t** vec, unsigned int s);

int16_t min(int16_t** vec, unsigned int s);

void inter_coef(int32_t *cd1, int32_t *cd2, int16_t* pesos, int16_t **prop, int16_t **prog, int16_t mxprop, int16_t mnprop, int16_t mxprog, int16_t mnprog, int16_t p, int16_t norma, unsigned int lim);

int16_t pond(int16_t *pesos, int16_t **prop, unsigned int s);

char trflags(int32_t** CD, unsigned int *flag, int sflag, unsigned int scd);

int coef_dist(int32_t*** CD, int16_t **prop, int16_t **prog, unsigned int sprop, unsigned int sprog, int16_t p, unsigned int *minJ);

int coef_dist2(int32_t*** CD, int16_t **prop, int16_t **prog, unsigned int sprop, unsigned int sprog, int16_t p, unsigned int *minJ);

#endif
