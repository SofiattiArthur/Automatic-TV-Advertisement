#include "coef_dist.h"
int RELEVANCIA;
int VOLMIN;

/**
 * DIFERENCIAL
 * Calcula a derivada de um vetor
 * @param x endereço do vetor de entrada
 * @param sx tamanho do vetor de entrada
 * @return Retorna o primeiro endereco do vetor de saida
 */
int *diffI(unsigned int *x, unsigned int sx)
{
  int *y;
  int i;

  y = (int*)malloc(sizeof(int)*(sx));
  if (y == NULL)
    return NULL;

  for (i=0;	i<(sx-1);	i++)
    y[i] = (int)(x[i+1]-x[i]);
  
  y[i] = 2;

  return y;
}

/**
 * Somatória de todos os valores de um vetor
 * @param vec endereço do vetor de entrada
 * @param t tamanho do vetor de entrada
 * @return Retorna o resultado da somatória
 */
int16_t sum(int16_t* vec, unsigned int t)
{
  int16_t s = 0;
  unsigned int i;

  for (i = 0; i < t; i++)
      s += vec[i];

  return s;
}

/**
 * Calcula a porcentagem de silencio em um intervalo
 * @param vec endereço da matriz de coeficientes MFCC cujo início coincide com o do intervalo de interesse
 * @param s tamanho do intervalo
 * @return Retorna um inteiro entre 0 e 100 referente a porcentagem de silêncio
 */
int silencio(int16_t** vec, unsigned int s)
{
  int i;
  int cont = 0;

  for (i = 0; i < s; i++)
  {
    //se a energia do vetor vec[i] for menor que o limite, o contador soma 1
    cont+=(vec[i][0]<LIMSIL);
  }

    return (cont*100)/s;
}

/**
 * calcula o maior termo de um vetor
 * @param vec endereço da matriz cuja primeira coluna forma o vetor a ser analisado
 * @param s tamanho do vetor
 * @return Retorna o volor do maior termo encontrado
 */
int16_t max(int16_t** vec, unsigned int s)
{
  int i;
  int16_t mvec;

  mvec = MAX(vec[0][0], vec[1][0]);
  for (i = 2; i < s; i++)
    mve c =MAX(mvec, vec[i][0]);

  return mvec;
}

/**
 * calcula o menor termo de um vetor
 * @param vec endereço da matriz cuja primeira coluna forma o vetor a ser analisado
 * @param s tamanho do vetor
 * @return Retorna o valor do menor termo encontrado
 */
int16_t min(int16_t** vec, unsigned int s)
{
  int i;
  int16_t mvec;

  mvec = MIN(vec[0][0], vec[1][0]);
  for (i = 2; i < s; i++)
    mvec=MIN(mvec, vec[i][0]);

  return mvec;
}

/**
 * calcula coeficientes em determinado intervalo
 * @param cd1 endereço do primeiro vetor de coeficientes com início coincidente com o do intervalo
 * @param cd2 endereço do segundo vetor de coeficientes com início coincidente com o do intervalo
 * @param pesos vetor de valores dos pesos de cada coluna dos vetores de coeficientes
 * @param prop endereço da matriz de coeficientes MFCC da propaganda analisada com início coincidente com o do intervalo
 * @param prog endereço da matriz de coeficientes MFCC da programação analisada com início coincidente com o do intervalo
 * @param mxprop maior valor de energia do vetor prop no intervalo analisado
 * @param mnprop menor valor de energia do vetor prop no intervalo analisado
 * @param mxprog maior valor de energia do vetor prog no intervalo analisado
 * @param mnprog menor valor de energia do vetor prog no intervalo analisado
 * @param p parâmetro de ponderação para comparação da coluna de energia e demais colunas relevantes
 * @param norma normalização dos coeficientes caso o intervalo analisado seja menor que o tamanho da propaganda
 * @param lim tamanho do intervalo
 */
void inter_coef(int32_t *cd1, int32_t *cd2, int16_t* pesos, int16_t **prop, int16_t **prog, int16_t mxprop, int16_t mnprop, int16_t mxprog, int16_t mnprog, int16_t p, int16_t norma, unsigned int lim){
    int i, j;
    int64_t cdAux1;
    int64_t cdAux2;
    int32_t aux;
    int16_t dist1;
    int16_t dist2;
    int16_t dp;
    int16_t dg;
    //Variaveis auxiliares
    cdAux1 = 0;
    cdAux2 = 0;

    dp = mxprop-mnprop;
    dg = mxprog-mnprog;
    dp += dp == 0;
    dg += dg == 0;
    for (i = 0; i < lim; i++)
    {
      //diferenca de energia
      dist1 = fabs((((int32_t)(prop[i][0]-mnprop)<<8)/dp) - (((int32_t)(prog[i][0]-mnprog)<<8)/dg));
      //diferença quadrática entre demais termos
      aux = 0;
      for (j=1;	j<RELEVANCIA;	j++)
      {
        dist2 = (prop[i][j] - prog[i][j]);
        aux += dist2*dist2;
      }
      dist2 = sqrt(aux);
      cdAux1 += (int64_t)pesos[i]*(((int64_t)p*dist1 + (256-p)*dist2));
      cdAux2 += (int64_t)pesos[i]*(((int64_t)(256-p)*dist1 + p*dist2));
    }
    cdAux1 >>= 16;
    cdAux2 >>= 16;
    cdAux1 *= norma;
    cdAux2 *= norma;

    (*cd1) = (int32_t)(cdAux1);
    (*cd2) = (int32_t)(cdAux2);
}

/**
 * Estima pesos para o cálculo dos coeficientes
 * @param pesos endereço do vetor onde serão guardados os pesos
 * @param prop endereço da matriz propaganda utilizada para o cálculo
 * @param s quantidade de pesos
 * @return Retorna a somatória dos pesos calculados
 */
int16_t pond(int16_t* pesos, int16_t** prop, unsigned int s)
{
  int i;
  int16_t SW = 0;

  for (i = 0; i < s; i++)
  {
    pesos[i]=trapmf(prop[i][0], -5120, -1280);
    SW+=pesos[i];
  }
  return SW;
}

/**
 * Tratamento das flags
 * Uma flag é registrada sempre que uma ocorrência de silêncio for detectada. Os
 * coeficientes desta região são transformados em uma reta para evitar grande diferença
 * de valores (o que é entendido como uma detecção) em uma região com pouca informação
 * @param CD endereço da matriz dos coeficientes iniciando na região de ocorrência das flags
 * @param flag endereço do vetor onde são registradas as flags
 * @param sflag quantidade de flags no vetor
 * @param scd numero máximo de colunas na matriz CD
 * @return Retorna 0 em caso de erro ou 1 caso tudo corra bem
 */
char trflags(int32_t** CD, unsigned int *flag, int sflag, unsigned int scd){
    int *dflag;
    int i, j, f;
    int32_t aux;

    dflag = diffI(flag, sflag);
    if (dflag == NULL)
      return 0;

    f = 0;

    for (i = 0; i < sflag; i++)
    {
      if (dflag[i] > 1)
      {
        if (flag[f] == 0)
        {
          for (j = flag[f]; j <= flag[i]; j++)
          {
            CD[0][j] = CD[0][flag[i]+1];
            CD[1][j] = CD[1][flag[i]+1];
          }
        }
        else if (flag[i] == (scd-1))
        {
          for (j = flag[f]; j <= flag[i]; j++)
          {
            CD[0][j] = CD[0][flag[i]-1];
            CD[1][j] = CD[1][flag[i]-1];
          }
        }
        else
        {
          for (j = flag[f]; j <= flag[i]; j++)
          {
            aux = ((int32_t)(j - MAX((int)0, (int)flag[f]-1))*65536/((MIN((int)scd-1, (int)flag[i]+1)) - MAX((int)0, (int)flag[f]-1)));
            CD[0][j] = aux*(CD[0][MIN((int)flag[i]+1, (int)scd-1)] - CD[0][MAX(0, (int)flag[f]-1)]) + CD[0][MAX(0, (int)flag[f]-1)];
            CD[1][j] = aux*(CD[1][MIN((int)flag[i]+1, (int)scd-1)] - CD[1][MAX(0, (int)flag[f]-1)]) + CD[1][MAX(0, (int)flag[f]-1)];
          }
        }
        f=i+1;
      }
    }

    if (dflag != NULL)
      free(dflag);

    return 1;
}

/**
 * Calcula a distância entre os coeficientes de uma propaganda e um corte de programação
 * @param cd matriz com dois vetores de coeficientes de diferenças
 * @param prop matriz dos coeficientes da propaganda
 * @param prog matriz dos coeficientes do programa
 * @param sprop numero de linhas da matriz prop
 * @param sprog numero de linhas da matriz prog
 * @param p parametro
 * @param minJ número minimo de janelas
 * @return Retorna 0 caso tudo corra bem ou outro valor de acordo com o erro
 * @return Retorna -1 caso haja erro na alocação de memória
 * @return Retorna -2 caso haja muito silêncio na propaganda
 * @return Retorna -3 caso haja erro no tratamento das flags
 */
int coef_dist(int32_t*** cd, int16_t **prop, int16_t **prog, unsigned int sprop, unsigned int sprog, int16_t p, unsigned int *minJ){
    unsigned int scd, *flag, sflag;
    unsigned int lip, lsp, lig, lsg;
    int32_t** CD, SW, sw;
    int16_t *pesos, aux;
    int16_t xprop, xprog, mxprop, mxprog, mnprop, mnprog;
    int i, j, cond;
    /******Inicialização******/
    pesos = NULL;
    flag = NULL;

    //número mínimo de janelas é ao menos um terço do tamanho da propaganda
    if ((sprop/3) > *minJ)
        *minJ = sprop/3;
    
    //número de colunas na matriz de saída
    scd = sprog + sprop - 2*(*minJ);

    //inicialização da matriz CD
    CD=(int32_t **) calloc(2, sizeof(int32_t*));
    if (CD == NULL) 
      return -1;

    for (i = 0; i < 2; i++)
    {
      CD[i]=(int32_t *) calloc(scd, sizeof(int32_t));
      if (CD[i] == NULL) 
        return -1;
    }

    //inicialização do vetor de pesos
    pesos = (int16_t*) calloc(sprop, sizeof(int16_t));
    if (pesos == NULL) 
      return -1;

    //inicialização do vetor de flags
    flag=(unsigned int*) calloc(scd, sizeof(unsigned int));
    if (flag == NULL)
      return -1;
    sflag = 0;

    //verificação da porcentagem de silêncio na propaganda
    xprop = silencio(prop, sprop);
    if ((xprop >= VOLMIN) || (sprop < (*minJ)))
      return -2;

    //cálculo dos pesos
    SW = (int32_t)pond(pesos, prop, sprop)<<8;

    /******Final da Inicialização******/

    /******Primeira parte******/
    j = sprop - (*minJ) - 1;
    i = 0;

    lip = LIP1;
    lsp = LSP1;
    lig = LIG1;
    lsg = LSG1;
    //maior termo da primeira coluna de prop
    mxprop = max(&prop[lip], (lsp-lip));

    //maior termo da primeira coluna de prog
    mxprog = max(&prog[lig], (lsg-lig));

    //menor termo da primeira coluna de prop
    mnprop = min(&prop[lip], (lsp-lip));

    //menor termo da primeira coluna de prog
    mnprog = min(&prog[lig], (lsg-lig));

    cond = (sprop-(*minJ)-1);
    for (; i < cond; i++, j--)
    {
      //limites comprop = [j sprop]
      //limites comprog = [0 sprop-j]
      //porcentagem de silencio em prop
      xprop = silencio(&prop[lip], (lsp-lip));

      if (xprop < VOLMIN)
      {
        //pesos
        sw = sum(&pesos[lip], (lsp-lip));

        //porcentagem de silencio em prog
        xprog = silencio(&prop[lig], (lsg-lig));

        if (xprog < VOLMIN)
        {
          //maior termo da primeira coluna de prop
          mxprop = MAX(prop[lip][0], mxprop);

          //maior termo da primeira coluna de prog
          mxprog = MAX(prog[lsg][0], mxprog);

          //maior termo da primeira coluna de prop
          mnprop = MIN(prop[lip][0], mnprop);

          //maior termo da primeira coluna de prog
          mnprog = MIN(prog[lsg][0], mnprog);

          //COEFICIENTES CD
          aux = SW/sw;
          inter_coef(&CD[0][i], &CD[1][i], &pesos[lip+lig], &prop[lip], &prog[lig], mxprop, mnprop, mxprog, mnprog, p, aux, (lsp-lip));
        }
        else
        {
          flag[sflag]=i;
          sflag++;
        }
      }
      else
      {
        flag[sflag]=i;
        sflag++;
      }
      lip--;
      lsg++;
    }//final da primeira parte

    /******Segunda parte******/
    //maior valor de prop;
    mxprop = MAX(prop[0][0], mxprop);

    //menor valor de prop;
    mnprop = MIN(prop[0][0], mnprop);

    //maior valor de prog
    mxprog = max(&prog[lig], (lsg-lig));

    //menor valor de prog
    mnprog = min(&prog[lig], (lsg-lig));

    j = 0;
    lip = LIP2;
    lsp = LSP2;
    lig = LIG2;
    lsg = LSG2;
    xprog = silencio(&prog[lig], (lsg-lig));
    cond = (scd - sprop + (*minJ));
    for (; i < cond; i++, j++)
    {
      //limites de comprop = [0 sprop]
      //limites de comprog = [j sprop+j]
      //porcentagem de silêncio em prog
      if (xprog<VOLMIN)
      {
        //COEFICIENTES CD
        inter_coef(&CD[0][i], &CD[1][i], &pesos[lip], &prop[lip], &prog[lig], mxprop, mnprop, mxprog, mnprog, p, 256, (lsp-lip));

        if (mxprog != prog[lig][0])
          mxprog = MAX(mxprog, prog[lsg][0]);
        else
          mxprog = max(&prog[lig+1], (lsg-lig));
        
        if (mnprog!=prog[lig][0])
          mnprog = MIN(mnprog, prog[lsg][0]);
        else
          mnprog = min(&prog[lig+1], (lsg-lig));
        
      }
      else
      {
        flag[sflag] = i;
        sflag++;
      }
      if (prog[lig][0] < LIMSIL)
        xprog -= 1/(lsg-lig);
      else
        xprog += 1/(lsg-lig);

      if (prog[lsg][0] < LIMSIL)
        xprog += 1/(lsg-lig);
      else
        xprog -= 1/(lsg-lig);
      
      lig++;
      lsg++;
    }//final da segunda parte
    /******Terceira parte******/
    //maior valor de prop
    mxprop = max(&prop[lip], (lsp-lip));

    //maior valor de prog
    mxprog = max(&prog[lig], (lsg-lig));

    //maior valor de prop
    mnprop = min(&prop[lip], (lsp-lip));

    //maior valor de prog
    mnprog = min(&prog[lig], (lsg-lig));

    j = sprop-1;
    lip = LIP3;
    lsp = LSP3;
    lig = LIG3;
    lsg = LSG3;
    for (; i < scd; i++, j--)
    {
      //limites de comprop = [0 j]
      //limites de comprog = [sprog-j sprog]
      //procentagem de silêncio em prop
      xprop = silencio(&prop[lip], (lsp-lip));

      if (xprop < VOLMIN && sprop > (*minJ))
      {
        //pesos
        sw = sum(&pesos[lip], (lsp-lip));

        //porcentagem de silêncio em prog
        xprog = silencio(&prog[lig], (lsg-lig));

        if (xprog < VOLMIN)
        {
          //COEFICIENTES CD
          aux = SW/sw;
          inter_coef(&CD[0][i], &CD[1][i], &pesos[lip+lip], &prop[lip], &prog[lig], mxprop, mnprop, mxprog, mnprog, p, aux, (lsp-lip));
          if (mxprop == prop[lsp-1][0])
            mxprop = max(&prop[lip], (MAX((lsp-1), 1)-lip));
          
          if (mnprop == prop[lsp-1][0])
            mnprop=min(&prop[lip], (MAX((lsp-1), 1)-lip));
          
          if (mxprog == prog[lig][0])
            mxprog = max(&prog[lig], (lsg-MIN((lig-1), scd-1)));
          
          if (mnprog == prog[lig][0])
            mnprog = min(&prog[lig], (lsg-MIN((lig-1), scd-1)));
        }
        else
        {
          flag[sflag] = i;
          sflag++;
        }
      }else{
          flag[sflag]=i;
          sflag++;
      }
      lsp--;
      lig++;
    }//final da terceira parte
    /******Flags******/
    if (sflag>0)
    {
      if (!trflags(CD, flag, sflag, scd))
        return -3;
    }//Final das flags

    /******Liberação da memória alocada******/
    if (flag != NULL)
        free(flag);

    if (pesos != NULL)
        free(pesos);

    *cd = CD;

    return 0;
}


/**
 * Calcula a distância entre os coeficientes de uma propaganda e um
 * corte de programação, mas apenas nas últimas janelas de
 * programação
 * @param cd matriz com dois vetores de coeficientes de diferença
 * @param prop matriz dos coeficientes da propaganda
 * @param prog matriz dos coeficientes do programa
 * @param sprop numero de linhas da matriz prop
 * @param sprog numero de linhas da matriz prog
 * @param p parametro
 * @param minJ número minimo de janelas
 * @return Retorna 0 caso tudo corra bem ou outro valor de acordo com o erro
 * @return Retorna -1 caso haja erro na alocação de memória
 * @return Retorna -2 caso haja muito silêncio na propaganda
 * @return Retorna -3 caso haja erro no tratamento das flags
 */
int coef_dist2(int32_t*** cd, int16_t **prop, int16_t **prog, unsigned int sprop, unsigned int sprog, int16_t p, unsigned int *minJ){
    unsigned int scd, *flag, sflag;
    unsigned int lip, lsp, lig, lsg;
    int32_t** CD, SW, sw;
    int16_t *pesos, aux;
    int16_t xprop, xprog, mxprop, mxprog, mnprop, mnprog;
    int i, j;
    /******Inicialização******/
    pesos=NULL;
    flag=NULL;

    //número mínimo de janelas é ao menos um terço do tamanho da propaganda
    //número de colunas na matriz de saída
    scd = sprog + sprop - 2*(*minJ);

    //inicialização da matriz CD
    CD=(int32_t **) calloc(2, sizeof(int32_t*));
    if (CD == NULL)
      return -1;

    for (i = 0; i < 2; i++)
    {
      CD[i] = (int32_t *) calloc(scd, sizeof(int32_t));
      if (CD[i] == NULL) 
        return -1;
    }

    //inicialização do vetor de pesos
    pesos = (int16_t*) calloc(sprop, sizeof(int16_t));
    if (pesos == NULL)
      return -1;

    //inicialização do vetor de flags
    flag = (unsigned int*) calloc(scd, sizeof(unsigned int));
    if (flag == NULL)
      return -1;
    sflag=0;

    //verificação da porcentagem de silêncio na propaganda
    xprop = silencio(prop, sprop);
    if ((xprop >= VOLMIN) || (sprop < (*minJ)))
      return -2;

    //cálculo dos pesos
    SW = (int32_t)pond(pesos, prop, sprop)<<8;

    /******Final da Inicialização******/

    i=(scd-sprop+(*minJ));

    j=sprop-1;
    lip=LIP3;
    lsp=LSP3;
    lig=LIG3;
    lsg=LSG3;

    //maior termo da primeira coluna de prop
    mxprop = max(&prop[lip], (lsp-lip));

    //maior termo da primeira coluna de prog
    mxprog = max(&prog[lig], (lsg-lig));

    //menor termo da primeira coluna de prop
    mnprop = min(&prop[lip], (lsp-lip));

    //menor termo da primeira coluna de prog
    mnprog = min(&prog[lig], (lsg-lig));

    for (; i < scd; i++, j--)
    {
      //limites de comprop = [0 j]
      //limites de comprog = [sprog-j sprog]
      //procentagem de silêncio em prop
      xprop = silencio(&prop[lip], (lsp-lip));

      if (xprop < VOLMIN && sprop > (*minJ))
      {
        //pesos
        sw = sum(&pesos[lip], (lsp-lip));

        //porcentagem de silêncio em prog
        xprog = silencio(&prog[lig], (lsg-lig));

        if (xprog < VOLMIN)
        {
          //COEFICIENTES CD
          aux = SW/sw;
          inter_coef(&CD[0][i], &CD[1][i], &pesos[lip+lip], &prop[lip], &prog[lig], mxprop, mnprop, mxprog, mnprog, p, aux, (lsp-lip));
          if (mxprop == prop[lsp-1][0])
            mxprop = max(&prop[lip], (MAX((lsp-1), 1)-lip));
          
          if (mnprop == prop[lsp-1][0])
            mnprop = min(&prop[lip], (MAX((lsp-1), 1)-lip));
          
          if (mxprog == prog[lig][0])
            mxprog = max(&prog[lig], (lsg-MIN((lig-1), scd-1)));
          
          if (mnprog == prog[lig][0])
            mnprog = min(&prog[lig], (lsg-MIN((lig-1), scd-1)));
        }
        else
        {
          flag[sflag]=i;
          sflag++;
        }
      }
      else
      {
        flag[sflag]=i;
        sflag++;
      }
      lsp--;
      lig++;
    }
    /******Flags******/
    if (sflag>0)
    {
      if (!trflags(CD, flag, sflag, scd))
        return -3;
    }//Final das flags

    /******Liberação da memória alocada******/
    if(flag != NULL)
      free(flag);
    
    if(pesos != NULL)
      free(pesos);

    *cd = CD;

    return 0;
}