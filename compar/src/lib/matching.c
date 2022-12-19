#include "matching.h"

int MINJ;
int VR_LIM;
int LIM_SINC;

/**
 * Encontra a ultima janela com audio correto em uma propaganda
 * @param prop primeiro endereço da matriz de propaganda
 * @param sp numero de linhas da matriz de propaganda
 * @return Retorna a primeira posicao da janela encontrada
 */
int find_end(int16_t **prop, unsigned int sp)
{
  int p;
  int xprop = 0;

  for (p = (sp-MINJ-1); p > (sp/2) && xprop<LIMSIL; p--)
    xprop = silencio(&prop[p], MINJ);
  
  return p;
}

/**
 * Inverte cada um dos termos de um vetor
 * @param x endereço da primeira posição do vetor de entrada
 * @param s tamanho do vetor
 * @return retorna endereço da primeira posição do vetor de resposta
 */
int32_t* invers(int32_t* x, unsigned int s)
{
  unsigned int i;
  int32_t *ans;

  ans = (int32_t*)malloc(sizeof(int32_t)*s);
  if (ans == NULL)
    return NULL;

  for (i = 0; i < s; i++)
  {
    x[i]+= x[i]==0;
    ans[i]=1073741824/x[i]; //fixou-se o ponto em 8bits para parte inteira e 24 para decimal
  }

  return ans;
}

/**
 * Calcula a diferencial de um vetor
 * @param vec endereço da primeira posição do vetor de entrada
 * @param s tamanho do vetor
 * @return retorna endereço da primeira posição do vetor de resposta
 */
int32_t* diff(int32_t* vec, unsigned int s)
{
  unsigned int i;
  int32_t *ans;

  s--;

  ans = (int32_t*)malloc(sizeof(int32_t)*s);
  if (ans == NULL)
    return NULL;

  for (i = 0; i < s; i++)
    ans[i] = (vec[i+1]-vec[i])<<10; //aumenta o ganho do vetor multiplicando por 1024

  return ans;
}

/**
 * Calcula um limiar de acordo com a média quadrática de um determinado vetor
 * @param vec endereço da primeira posição do vetor de entrada
 * @param s tamanho do vetor
 * @return retorna o valor calculado para o limiar
 */
int32_t limiar(int32_t *vec, unsigned int s)
{
  int32_t lim;
  int64_t aux;
  unsigned int i;

  aux=0;
  for (i = 0; i < s; i++)
      aux += (int64_t)vec[i]*vec[i];

  aux /= s;
  lim = DL*sqrt(aux);

  return lim;
}

/**
 * Encontra o maior valor em um vetor
 * @param vec endereço da primeira posição do vetor de entrada
 * @param s tamanho do vetor
 * @param k posição onde foi encontrado o maior valor
 * @return retorna o maior valor encontrado
 */
int32_t maximus(int32_t* vec, unsigned int s, unsigned int* k)
{
  int i;
  int32_t mvec;

  *k = 0;

  if (s == 0)
    return NAN;
  
  if (s == 1)
    return vec[0];

  mvec=vec[0];
  for (i = 1; i < s; i++)
  {
    if(vec[i]>mvec)
    {
      *k = i;
      mvec = vec[i];
    }
  }

  return mvec;
}

/**
 * Encontra o menor valor em um vetor
 * @param vec endereço da primeira posição do vetor de entrada
 * @param s tamanho do vetor
 * @param k posição onde foi encontrado o menor valor
 * @return retorna o menor valor encontrado
 */
int32_t minimus(int32_t* vec, unsigned int s, unsigned int* k)
{
  int i;
  int32_t mvec;

  *k = 0;

  if (s == 0)
    return NAN;
  
  if (s == 1)
    return vec[0];

  mvec=vec[0];
  for (i = 1; i < s; i++)
  {
    if(vec[i] < mvec)
    {
      *k = i;
      mvec = vec[i];
    }
  }

  return mvec;
}

/**
 * Calcula o instante em que iniciou a ocorrência encontrada com base na posição da ocorrência
 * nos vetores cd e no tamanho da propaganda
 * @param k posição da ocorrência nos vetores CD
 * @param s tamanho da propaganda
 * @return Retorna em milissegundos o instante em a propaganda começou
 */
int tempo(int k, unsigned int s)
{
  int kk, t;
  if (k < (s))
  {
    kk = k-1;
    t = -((s-kk)*64);  //(ms)
  }
  else
  {
    kk = k - (s);
    t = (kk*64); //(ms)
  }

  return t;
}

/**
 * Verifica se uma possivel ocorrencia é verdadeira e, em caso afirmativo, retorna o instante em que
 * ela foi registrada e a confiabilidade
 * @param CD coeficientes da comparação entre prop e prog
 * @param scd número de colunas em CD
 * @param i posição onde foi encontrada a possível ocorrência
 * @param limiar1 condição mínima para o primeiro coeficiente indicar uma ocorrência
 * @param limiar2 condição mínima para o segundo coeficiente indicar uma ocorrência
 * @param vr posição de saída para a confiabilidade da ocorrência
 * @return Retorna -1 caso o segundo coeficiente não corrobore a ocorrencia ou o tempo em milissegundos
 */
int ocurr(int32_t** CD, unsigned int scd, unsigned int i, int32_t limiar1, int32_t limiar2, int* vr)
{
  unsigned int a, b, j, n, k, p1, p2, sdg;
  int t;
  int vr1, vr2;
  int32_t m1, m2;

  n = JAN;

  maximus(&CD[0][i], MIN(n+1, scd-1-i), &k);
  k += i;

  a = (unsigned int) MAX((int)(k-n), (int)0);
  b = (unsigned int) MIN((int)(k+n), (int)(scd-1));
  sdg = b-a;

  m1 = abs(maximus(&CD[1][a], sdg, &p1));
  m2 = abs(minimus(&CD[1][a], sdg, &p2));

  if ((MIN(m1, m2) <= limiar2))
    return -1;   //Caso o segundo coeficiente não corrobore a ocorrencia

  vr1 = 1000 - ((1000*limiar2)/m1);
  vr2 = 1000 - ((1000*limiar2)/m2);

  *vr = MAX(vr1, vr2);
  t = k;

  for (j = a; j < b; j++)
  {
    CD[0][j]=limiar1;
    CD[1][j]=limiar2;
  }
  if(*vr < VR_LIM)
    t = -1;
  
  return t;
}

/**
 * Inverte e tira a derivada dos vetores CD
 * @param CD Primeiro endereço de uma matriz de vetores CD
 * @param sp tamanho da propaganda
 * @param sg tamanho do corte de programaçao
 * @param minJ numero minimo de janelas
 * @return Retorna o tamanho da matriz CD apos as operaçoes
 */
int CDtrat(int32_t** CD, unsigned int sp, unsigned int sg, unsigned int minJ)
{
  int scd, i;
  int32_t *aux, *aux2;

  scd = (sp) + (sg) - 2*minJ;

  aux = invers(CD[0], scd);
  if (aux == NULL)
    return -1;

  aux2 = diff(aux, scd);
  if (aux2 == NULL)
    return -1;

  for(i = 0; i < scd; i++)
    CD[0][i]=aux2[i];

  free(aux);
  free(aux2);

  aux = invers(CD[1], scd);
  if (aux == NULL)
    return -1;

  aux2 = diff(aux, scd);
  if (aux2 == NULL)
    return -1;

  for(i = 0; i < scd; i++)
    CD[1][i] = aux2[i];

  free(aux);
  free(aux2);

  scd--;
  return scd;
}

/**
 * Identifica ocorrencias de propagandas na programação. Informa instante da
 * ocorrência e confiabilidade.
 * @param prop coeficientes MFCC da propaganda
 * @param prog coeficientes MFCC da programação
 * @param sp numero de linhas da matriz prop
 * @param sg numero de linhas da matriz prog
 * @param t vetor de saída com os instantes em que ocorreram as ocorrências
 * @param vr vetor de saída com os níveis de confiabilidade de cada ocorrência
 * @param flag para procurar a propaganda cortada: se 1 procura; se diferente de 1 não procura pedaços da propaganda, apenas ela inteira. (usado para procurar o inicio da propaganda no anterior)
 * @param g flag referente ao final da propaganda
 * @return retorna o número de ocorrências, em caso de erro retorna:
 * @return -1 = erro na alocação de memória
 * @return -2 = caso haja muito silêncio na propaganda
 * @return -3 = erro no tratamento das flags
 */
int matching(int16_t** prop, int16_t** prog, unsigned int sp, unsigned int sg, int* t, int* vr, char flag, unsigned char* g)
{
  int32_t limiar1, limiar2, limiar1a, limiar2a;
  int16_t p;
  int32_t ***auxcd, **CD;
  int err;
  int taux, vraux, vrLIMaux;
  unsigned int minJ, minJa, scd, scdaux;
  unsigned int i, j, m;

  //INICIALIZAÇÃO
  CD    = NULL;
  minJ  = MINJ;
  auxcd = (int32_t***)malloc(sizeof(int32_t**));
  if (auxcd==NULL) return -1;
  //FINAL DA INICIALIZAÇÃO

  //Calculo dos coeficientes
  if (flag==1)
      err = coef_dist(auxcd, &prop[2], &prog[2], sp-4, sg-4, P, &minJ);
  else
      err = coef_dist2(auxcd, &prop[2], &prog[2], sp-4, sg-4, P, &minJ);
  
  if (err!=0)
    return err; //Caso deseje-se procurar ocorrências mesmo com muito silêncio, alterar essa linha

  CD    = (*auxcd);
  auxcd = NULL;

  scd=CDtrat(CD, sp-4, sg-4, minJ);
  if (scd<0) return scd;

  limiar1 = limiar(CD[0], scd);
  limiar2 = limiar(CD[1], scd);

  p = find_end(prop, sp);
  m = 0;

  g[0] = 0;
  for (i = 0; i < scd; i++)
  {
    if (CD[0][i] > limiar1)  // encontrou uma possível ocorrência
    {
      t[m] = ocurr(CD, scd, i, limiar1, limiar2, &vr[m]);
      if (t[m] >= 0) 
      {
        minJa = MINJ - 1;
        if (auxcd == NULL)
          auxcd = (int32_t ***) malloc(sizeof(int32_t**));

        if (auxcd == NULL) 
        {
          g[m] = ERRO_G;
          continue;
        }

        j = MIN((sg - 2*sp - 2), t[m]);
        err = coef_dist(auxcd, &prop[p], &prog[j], MINJ, sp*2, P, &minJa);
        if (err != 0)
        {
          g[m] = ERRO_G;
          continue;
        }
        scdaux = CDtrat(*auxcd, MINJ, 2*sp, minJa);
        if (scdaux < 0) 
        {
          g[m] = ERRO_G;
          continue;
        }
        limiar1a = limiar((*auxcd)[0], scdaux);
        limiar2a = limiar((*auxcd)[1], scdaux);
        for (j = 0; j < scdaux; j++) 
        {
          if ((*auxcd)[0][j] > limiar1a)
          {
            vrLIMaux = VR_LIM;
            VR_LIM = 0;
            taux = ocurr(*auxcd, scdaux, j, limiar1a, limiar2a, &vraux);
            VR_LIM = vrLIMaux;
            if (taux >= 0) 
            {
              if(sp/3 > MINJ) 
              {
                if (abs(sp/3 - taux - MINJ) < LIM_SINC)
                {
                    g[m] = FIND_G;
                    j = scdaux;
                }
              } 
              else if (abs(sp/2 - taux - MINJ) < LIM_SINC)
              {
                g[m] = FIND_G;
                j = scdaux;
              }
            }
          }
        }
        if ((*auxcd)[0] != NULL)
          free((*auxcd)[0]);

        if ((*auxcd)[1] != NULL)
          free((*auxcd)[1]);
        
        t[m] = tempo(t[m], sp-minJ);
        m++;
        g[m] = N_FIND_G;
      }
    }
  }
  if (CD[0] != NULL)
    free(CD[0]);

  if (CD[1] != NULL)
    free(CD[1]);

  if (CD != NULL)
    free(CD);
    
  if (auxcd != NULL)
    free(auxcd);

  return m;
}