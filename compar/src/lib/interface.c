//
// Created by arthurf on 04/05/16.
//

/*Exemplo de entrada:
 *  $ comparacao /CAMINHO/PARA/LISTA/PROPAGANDA.txt /CAMINHO2/PARA/ALVO.mfc -p /CAMINHO2/PARA/ALVO/ANTERIOR.mfc
 *  NOME/DO/ARQUIVO/A/SER/CRIADO.csv
 *
 *  $ comparacao /CAMINHO/PARA/LISTA.txt /CAMINHO2/PARA/ALVO.mfc NOME/DO/ARQUIVO/A/SER/CRIADO.csv
 *
 *  Lista de Erros:
 *
 *   001 - Erro na extensão do arquivo de propaganda
 *   002 - Erro na abertura do arquivo de propaganda
 *   003 - Propaganda com demasiado silêncio
 *   004 - Erro na extensão do arquivo de programação
 *   005 - Parametros do cabecalho dos arquivos .mfc não coincidem
 *   008 - Erro na abertura do arquivo de programação
 *   009 - Erro na comparação dos coeficientes
 *   016 - Erro na extensão do arquivo de saída
 *   032 - Erro na abertura do arquivo de saída
 *   064 - Erro na extensão do arquivo de programação anterior
 *   128 - Erro na abertura do arquivo de programação anterior
 *   192 - Erro no parâmetro de arquivo de programação anterior
 *   255 - Entrada invalida
 *
**/

#include "../include/interface.h"
char EXT_LIST[];
char EXT_MFCC[];
char EXT_DEST[];
int TIME_DIF;

int MINJ;
int VR_LIM;
int LIM_SINC;

int RELEVANCIA;
int VOLMIN;

int RELEVANCIA_aux;

/**
 * Verifica se um inteiro se encontra entre dois valores
 * @param valor inteiro que será comparado
 * @param li limite inferior do intervalo
 * @param ls limite superior do intervalo
 * @return Retorna 0 se o valor estiver entre o limite inferior (>=) e o limite superior (<=), ou 1 caso contrário
 */
int testa_inteiro(int valor, int li, int ls)
{
  if (valor >= li && valor <= ls)
    return 0;

  return 1;
}

/**
 * Testa os parâmetros recebidos de um arquivo de configuração
 * @return Retona 2^n, caso o enésimo termo em ordem de aparição no
 * @return arquivo .conf esteja errado, caso mais de um termo esteja
 * @return errado o retorno é o somatório de 2^a + 2^b +..., caso não
 * @return haja nenhum erro, retorna 0
 */
int testa_conf()
{
  int flag = 0;

  if (testa_inteiro(TIME_DIF, 0, 60000))
  {
    TIME_DIF = TIME_DIF_DEF;
    flag  = 1;
  }
  
  if (testa_inteiro(LIM_SINC, 0, 60000))
  {
    LIM_SINC = LIM_SINC_DEF;
    flag |= (1<<1);
  }
  
  if (testa_inteiro(MINJ, 0, 250))
  {
    MINJ = MINJ_DEF/64;
    flag |= (1<<2);
  }
  
  if (testa_inteiro(VR_LIM, 0, 1000))
  {
    VR_LIM = VR_LIM_DEF;
    flag |= (1<<5);
  }
  
  if (testa_inteiro(RELEVANCIA, 2, 64))
  {
    RELEVANCIA += RELEVANCIA_DEF;
    flag |= (1<<6);
  }
  
  if (testa_inteiro(VOLMIN, 0, 100))
  {
    VOLMIN = VOLMIN_DEF;
    flag |= (1<<8);
  }

  return flag;
}

/**
 * Cria um novo arquivo de configuração
 * @param name nome do arquivo
 * @return Retorna 1 em caso de erro ou 0 caso contrário
 */
int new_conf(char* name)
{
  FILE *conf;
  sprintf(EXT_LIST, "%s", EXT_LIST_DEF);
  sprintf(EXT_MFCC, "%s", EXT_MFCC_DEF);
  sprintf(EXT_DEST, "%s", EXT_DEST_DEF);
  TIME_DIF = TIME_DIF_DEF;
  LIM_SINC = LIM_SINC_DEF/64;

  MINJ = MINJ_DEF/64;
  VR_LIM = VR_LIM_DEF;

  RELEVANCIA = RELEVANCIA_DEF;
  VOLMIN = VOLMIN_DEF;

  conf = fopen(name, "w");
  if (conf == NULL) 
    return 1;

  fprintf(conf, "#EXTENCAO DOS ARQUIVOS DE LISTA\n");
  fprintf(conf, "EXT_LISTA = %s\n", EXT_LIST_DEF);
  fprintf(conf, "#EXTENCAO DOS ARQUIVOS MFCC\n");
  fprintf(conf, "EXT_MFCC = %s\n", EXT_MFCC_DEF);
  fprintf(conf, "#EXTENCAO DOS ARQUIVOS DE SAIDA\n");
  fprintf(conf, "EXT_DEST = %s\n", EXT_DEST_DEF);
  fprintf(conf, "#DESSINCRONIZAÇÃO TOLERADA NO INICIO DA PROPAGANDA\n");
  fprintf(conf, "TIME_DIF = %d #(ms)\n", TIME_DIF_DEF);
  fprintf(conf, "#DESSINCRONIZAÇÃO TOLERADA NO FINAL DA PROPAGANDA\n");
  fprintf(conf, "END_DIF = %d #(ms)\n\n", LIM_SINC_DEF);

  fprintf(conf, "#MINIMO DE JANELAS\n");
  fprintf(conf, "MIN_JAN = %d #(ms)\n", MINJ_DEF);
  fprintf(conf, "#PRECISÃO MINIMA\n");
  fprintf(conf, "VR_MIN = %d\n\n", VR_LIM_DEF);

  fprintf(conf, "#NUMERO DE COEFICIENTES MFCC UTILIZADOS\n");
  fprintf(conf, "RELEVANCIA = %d\n", RELEVANCIA_DEF);
  fprintf(conf, "#PORCENTAGEM MINIMA DE AUDIO SIGNIFICATIVO\n");
  fprintf(conf, "VOL_MIN = %d\n", VOLMIN_DEF);

  fclose(conf);
  return 0;
}

/**
 * Carrega os parâmetros salvos em um arquivo de configuração
 * @param name nome do arquivo
 * @return 0 - caso tudo corra bem
 * @return 1 - caso algum dos parâmetros do arquivo de configuração esteja errado
 * @return 2 - caso ocorra o erro 1 e não seja possível criar um novo arquivo
 * @return 3 - caso não seja possível abrir o arquivo de configuração
 * @return 4 - caso ocorra o erro 3 e não seja possível criar um novo arquivo
 */
int load_conf(char* name)
{
  int flag;
  cfg_t *cfg;
  cfg_opt_t opts[] = {
    CFG_STR("EXT_LISTA", EXT_LIST_DEF, CFGF_NONE),
    CFG_STR("EXT_MFCC", EXT_MFCC_DEF, CFGF_NONE),
    CFG_STR("EXT_DEST", EXT_DEST_DEF, CFGF_NONE),
    CFG_INT("TIME_DIF", TIME_DIF_DEF, CFGF_NONE),
    CFG_INT("END_DIF", LIM_SINC_DEF/64, CFGF_NONE),

    CFG_INT("MIN_JAN", MINJ_DEF/64, CFGF_NONE),
    CFG_INT("VR_MIN", VR_LIM_DEF, CFGF_NONE),

    CFG_INT("RELEVANCIA", RELEVANCIA_DEF, CFGF_NONE),
    CFG_INT("VOL_MIN", VOLMIN_DEF, CFGF_NONE),

    CFG_END()
  };

  cfg = cfg_init(opts, CFGF_NONE);
  flag = cfg_parse(cfg, name);
  if (flag == CFG_PARSE_ERROR)
  {
    if (new_conf(name)) 
      return 2;
    return 1;
  }
  else if (flag == CFG_FILE_ERROR)
  {
    if (new_conf(name)) 
      return 4;
    return 3;
  }

  sprintf(EXT_LIST, "%s", cfg_getstr(cfg, "EXT_LISTA"));
  sprintf(EXT_MFCC, "%s", cfg_getstr(cfg, "EXT_MFCC"));
  sprintf(EXT_DEST, "%s", cfg_getstr(cfg, "EXT_DEST"));
  TIME_DIF = cfg_getint(cfg, "TIME_DIF");
  LIM_SINC = cfg_getint(cfg, "END_DIF");
  LIM_SINC /= 64;

  MINJ = cfg_getint(cfg, "MIN_JAN");
  MINJ /= 64;
  VR_LIM = cfg_getint(cfg, "VR_MIN");

  RELEVANCIA = cfg_getint(cfg, "RELEVANCIA");
  VOLMIN = cfg_getint(cfg, "VOL_MIN");

  cfg_free(cfg);
  return 0;
}

/**
 * Procura por determinado parametro em meio a um vetor de strings
 * @param argc tamanho do vetor
 * @param argv vetor de strings
 * @return a posicao do parametro procurado no vetor de strigs ou -1 caso nao encontre
 */
int conf_par(int argc, char** argv)
{
  int i;
  for (i = 1; i < (argc-1); i++)
  {
    if (!strcmp(CONF_PARAM, argv[i]))
      return (i);
  }
  return 0;
}

/**
 * Desloca os termos de um vetor de strings
 * @param argc tamanho do vetor
 * @param argv vetor de strings
 * @param p posicao a partir da qual os termos serao deslocados
 */
void desloca_argv(int argc, char **argv, int p)
{
  int i;
  for (i = p+2; i < argc; i++)
    strcpy(argv[i-2], argv[i]);
}

/**
 * Faz todas letras de uma string serem minúsculas
 * @param str ponteiro para string de entrada
 */
void tolow(char* str)
{
  unsigned int i;
  for (i=0;   str[i]!='\0';   i++)
  {
    if (str[i]>0x40 && str[i]<0x5B)
      str[i]+=0x20;
  }
}

/**
 * Encontra a posição da extensão em uma string
 * @param str primeiro endereço de memória da string de entrada
 * @param ext primeiro endereço de memória da string de extensão
 * @return Retorna a primeira posição relativa da extensão na string
 */
char* stristr(char* str, char* ext)
{
  char *a, aux[MAXN];
  a = strstr(str, ext);
  if (a == NULL)
  {
    strcpy(aux, str);
    tolow(aux);
    a = strstr(aux, ext);
  }
  return a;
}

/**
 * Verifica a presença de uma extensão em uma string
 * @param str primeiro endereço de memória da string de entrada
 * @param ext primeiro endereço de memória da string de extensão
 * @return Retorna o valor da posição relativa da extensão ou -1 caso não encontre
 */
int testa_extensao(char *str, char *ext)
{
  char *a;
  a = stristr(str, ext);
  if(a == NULL) 
    return(-1);

  if(strcmp(a, ext)!=0) 
    return (-1);

  return (strlen(str)-strlen(ext));
}

/**
 * Abre um arquivo de acordo com sua extensão e verifica erros
 * @param arq ponteiro para arquivo que será carregado
 * @param name primeiro endereço de memória da string de nome do arquivo
 * @param ext primeiro endereço de memória da string de extensão
 * @return Retorna 0 caso tudo corra bem ou um outro valor de acordo com o erro
 */
unsigned char testa_arquivo(FILE** arq, char* name, char* ext)
{
  (*arq) = NULL;
  if (testa_extensao(name, ext)<=0)
    return ERR_EXT;

  if (!strcmp(ext, EXT_LIST) || !strcmp(ext, EXT_MFCC))
    (*arq) = fopen(name, "r");
  else
    (*arq) = fopen(name, "w");
  
  if ((*arq) == NULL) 
    return ERR_OPN;
  
  return 0;
}

/**
 * Interpreta a entrada do usuário
 * @param argc número de strings passadas na chamada do programa
 * @param argv vetor de strings passadas na chamada do programa
 * @param lista ponteiro para o arquivo com a lista de propagandas
 * @param prog ponteiro para o arquivo com os coeficientes da programação
 * @param pprog ponteiro para o arquivo com os coeficientes da programação anterior
 * @param out ponteiro para o arquivo onde será escrito o relatório de saída
 * Retorna 0 caso tudo corra bem ou outro valor de acordo com o erro que ocorreu
 */
unsigned char interpIn(int argc, char** argv, FILE** lista, FILE** prog, FILE** pprog, FILE** out)
{
  unsigned char ans, i;
  ans = 0;
  i = 0;
  switch (argc)
  {
    case (SEM_ANTERIOR):   //sem alvo anterior
      ans |= testa_arquivo(lista, argv[i], EXT_LIST);
      i++;
      ans |= testa_arquivo(prog, argv[i], EXT_MFCC)<<2;
      i++;
      ans |= testa_arquivo(out, argv[i], EXT_DEST)<<4;
      break;

    case (COM_ANTERIOR):   //com alvo anterior
      ans |= testa_arquivo(lista, argv[i], EXT_LIST);
      i++;
      ans |= testa_arquivo(prog, argv[i], EXT_MFCC)<<2;
      i++;
      if(strcmp(PPARAM, argv[i]))
          ans |= ERR_PAR; //erro de parâmetro inválido
      i++;
      ans |= testa_arquivo(pprog, argv[i], EXT_MFCC)<<6;
      i++;
      ans |= testa_arquivo(out, argv[i], EXT_DEST)<<4;
      break;

    default:
      return ERR_ENT; //Erro de entrada inválida
      break;
  }
  return ans;
}

/**
 * Verifica a presença dos bites de identificação de arquivos do tipo .mfc
 * @param fil arquivo alvo
 * @return Retorna 0 caso não haja os bites de intentificação ou 1 caso haja
 */
int MAGIC(FILE* fil)
{
  unsigned char a;
  a = getc(fil);
  if (a!='M') 
    return 0;

  a = getc(fil);
  if (a!='F') 
    return 0;

  a = getc(fil);
  if (a!='C') 
    return 0;
  
  a = getc(fil);
  if (a!='C') 
    return 0;
  
  return 1;
}

/**
 * Trata um determinado valor de erro
 * @param inType valor de erro de entrada
 * @param erros vetor onde são registrados os possíveis erros ocorridos durante a execução da função
 * @return Retorna o número de erros que ocorreram (0 é ok)
 */
int err2(unsigned char inType, char* erros)
{
  unsigned char i;
  int n=0;
  if (inType == ERR_ENT)
  {
    erros[n] = ERR_ENT;
    n++;
    return n;
  }
  for (i = 0; i < 8; i += 2)
  {
    switch ((inType & (unsigned char)(0x03<<i))>>i)
    {
      case ((unsigned char)ERR_EXT):
        erros[n] = (ERR_EXT<<i);
        n++;
        break;
    
      case ((unsigned char)ERR_OPN):
        erros[n] = (ERR_OPN<<i);
        n++;
        break;
    
      case ((unsigned char)ERR_PAR):
        erros[n] = (ERR_PAR<<i);
        n++;
        break;
    
      default:
        break;
    }
  }
  return n;
}

/**
 * Trata erros vindos da comparação dos coeficientes
 * @param n saida da funcao de comparacao dos coeficientes
 * @param erros vetor onde são registrados os possíveis erros ocorridos durante a execução da função
 * @return Retorna 1
 */
int errmt(int n, char* erros)
{
  switch (n)
  {
    case (-1):
      (*erros) = ERR_MTC;
      break;

    case (-2):
      (*erros) = ERR_SIL;
      break;

    case (-3):
      (*erros) = ERR_MTC;
      break;

    default:
      break;
  }
  return 1;
}

/**
 * Escreve relatório sobre a comparação entre determinados arquivos de propaganda e programação
 * @param out arquivo onde será escrita a saída
 * @param prop primeiro endereço da string com o nome do arquivo de propaganda
 * @param prog primeiro endereço da string com o nome do arquivo de programação
 * @param t primeira posição de um vetor com valores de tempo (em ms) onde detectou-se ocorrências
 * @param vr primeira posição de um vetor com valores de confiabilidade de cada ocorrência
 * @param flag primeiro posição de um vetor com flags que informam sobre o início da propaganda
 * @param prev flag referente a procura de parte da propaganda no arquivo anterior ao alvo
 * @param g flag referente ao final da propaganda
 * @param n número de ocorrências detectadas
 */
void RelGen(FILE* out, char* prop, char* prog, int* t, int* vr, unsigned char* flag, unsigned char* prev, unsigned char* g, unsigned int n)
{
  int i, min, s;

  for (i = 0; i < n; i++)
  {
      min = t[i]/60000;
      s = (t[i]-min*60000)/1000;
      fprintf(out, "%s;%s;%d;%d;%d;%02d:%02d;%03d;\n",prog, prop,flag[i],prev[i],g[i],min,s,vr[i]);
  }
}

/**
 * Carrega uma string de um arquivo e muda sua última posição para '\0'
 * @param fname primeira posição da string de saída
 * @param s tamanho máximo da string de saída
 * @param fil arquivo de onde será carregada a string
 * @return Retorna 0 em caso de erro ou 1 caso tudo corra bem
 */
int myfgets(char* fname, unsigned int s, FILE* fil)
{
  if (fscanf(fil, "%s", fname)==-1) 
    return 0;
  
  return 1;
}

/**
 * Copia uma matriz de coeficientes MFCC
 * @param in primeira posição da matriz de entrada
 * @param out primeira posição da matriz de saída
 * @param s número de linhas da matriz de entrada
 * @param nCoef número de colunas da matriz de entrada
 */
void cpyMFCC(int16_t** in, int16_t** out, unsigned int s, unsigned int nCoef)
{
  unsigned int i, j;
  for (i = 0; i < s; i++)
  {
    for (j = 0; j < nCoef; j++)
      out[i][j] = in[i][j];
  }
}

/**
 * Compara número de coeficientes da FFT, step e número de coeficientes da MFCC de dois arquivos .mfc
 * @param C1 cabecalho do arquivo .mfc 1
 * @param C2 cabecalho do arquivo .mfc 1
 * @return Retorna 0 caso sejam iguais ou 1 caso sejam diferentes
 */
char testa_parametros(cabecalho C1, cabecalho C2)
{
  if (C1.n_fft != C2.n_fft)  
    return 1;

  if (C1.step != C2.step)   
    return 1;

  if (C1.n_coef != C2.n_coef) 
    return 1;

  return 0;
}

/**
 * Tratamento de erro caso os parâmetros de dois arquivos .mfc não coincidam
 * @param erros vetor onde são registrados os possíveis erros ocorridos durante a execução da função
 * @return Retorna 1
 */
int err_param(char* erros)
{
  (*erros) = 5;
  return 1;
}

/**
 * Carrega os dados de um arquivo do tipo .mfc
 * @param fil arquivo alvo
 * @param C ponteiro para o cabecalho do arquivo que guarda informações de: n_fft, step, n_coef, comp, data_size.
 * @return Retorna o primeiro endereço de memória da matriz de coeficientes MFCC carregados
 */
int16_t **loadFile(FILE* fil, cabecalho *C)
{
  unsigned int i, j;
  u_int32_t aux;
  int16_t aux2;
  int16_t** data;

  if (!MAGIC(fil)) 
    return NULL;

  fread(&C->n_fft, 1, sizeof(C->n_fft), fil);
  fread(&C->step, 1, sizeof(C->step), fil);
  fread(&C->n_coef, 1, sizeof(C->n_coef), fil);
  fread(&C->comp, 1, sizeof(C->comp), fil);
  if (C->comp < RELEVANCIA)
    RELEVANCIA = C->comp;
    
  fread(&aux, 1, sizeof(aux), fil);
  C->data_size = aux/((C->n_coef)*(sizeof(int16_t))); //size, é o numero de linhas da matriz

  data = (int16_t**)calloc(sizeof(int16_t*), (C->data_size));
  if (data == NULL) 
    return NULL;

  for (i = 0; i < (C->data_size) && !feof(fil); i++)
  {
    data[i] = calloc(sizeof(int16_t*), (RELEVANCIA));
    if (data[i] == NULL) 
      return NULL;

    for (j = 0; j < C->comp && !feof(fil); j++) 
    {
      if (j < RELEVANCIA)
        fread(&data[i][j], sizeof(int16_t), 1, fil);
      else
        fread(&aux2, sizeof(int16_t), 1, fil);
    }
  }
  return data;
}

/**
 * Carrega um corte de programação imediatamente anterior ao alvo
 * @param Cpg cabecalho do corte que esta sendo carregado
 * @param Cg cabecalho do alvo
 * @param pprog matriz de coeficientes do corte que esta sendo carregado
 * @param pprogf arquivo .mfc que esta sendo carregado
 * @param erros vetor onde são registrados os possíveis erros ocorridos durante a execução da função
 * @param flagE tamanho do vetor erros
 * @return Retorna COM_ANTERIOR caso tudo corra bem
 * @return Retorna SEM_ANTERIOR em caso de erro
 */
int loadAnt(cabecalho *Cpg, cabecalho Cg, int16_t*** pprog, FILE *pprogf, char* erros, int* flagE) {
    *pprog = loadFile(pprogf, Cpg);
    if (pprog == NULL) 
      return SEM_ANTERIOR;

    if (testa_parametros(*Cpg, Cg))
    {
      (*flagE) += err_param(erros+(*flagE));
      return SEM_ANTERIOR;
    else
      return COM_ANTERIOR;
}

/**
 * Compara arquivos .mfc de propagandas e de programação de acordo com a entrada do usuário
 * Gera um relatório informando das correspondências encontradas
 * @param argc número de strings passadas junto a chamada do programa
 * @param argv primeira posição do vetor de strings passadas junto a chamada do programa
 * @param erros vetor que guarda os valores de erros que ocorreram durante a rotina, sendo que os quatro primeiros termos referem-se a erros de inicializacao e os proximos referem-se cada um a uma das propagandas na lista.
 * @return Retorna o numero de erros que ocorreram durante a rotina.
 */
int comparacao(int argc, char** argv, char *erros)
{
  FILE *propf, *progf, *pprogf, *out, *lista;
  char propName[MAXN], progName[MAXN], confName[MAXN];
  int n, pn, i, j, flagE;
  unsigned char inType, flag[MXOCUR], prev[MXOCUR], g[MXOCUR], ga[MXOCUR];
  int16_t **prop, **prog, **pprog;
  cabecalho Cg, Cp,Cpg;
  int vr[MXOCUR], t[MXOCUR], pvr[MXOCUR], pt[MXOCUR];

  flagE = 0;

  for (i = 0; i < NEI; i++)
    erros[i] = 0;

  if (!(argc%2)) 
  {
    flagE += err2(ERR_ENT, erros+flagE);
    return flagE; //1
  }

  if ((i=conf_par(argc, argv))){
    strcpy(confName, argv[i+1]);
    desloca_argv(argc, argv, i);
    argc -= 2;
  else
    strcpy(confName, CONFNAME);
  
  if (load_conf(confName))
  {
    erros[flagE] = 6;
    flagE++;
    return flagE; //1
  }
  if (testa_conf())
  {
    new_conf(confName);
    erros[flagE] = 6;
    flagE++;
    return flagE; //1
  }
  inType = interpIn(argc, argv, &lista, &progf, &pprogf, &out);

  //verificação de erro
  if (inType!=0)
  {
    flagE += err2(inType, erros+flagE);
    return flagE; //<=4
  }

  sprintf(progName,"%s",argv[1]);
  prog=loadFile(progf, &Cg);
  if(prog==NULL)
  {
    erros[flagE] = 8;
    flagE++;
    return flagE; //1
  }
  if (argc==COM_ANTERIOR)
  {
    argc = loadAnt(&Cpg, Cg, &pprog, pprogf, erros, &flagE);
    if (argc != COM_ANTERIOR) return flagE;//1
  }
  RELEVANCIA_aux = RELEVANCIA;
  for (i = 0; !feof(lista); i++)
  {
    erros[i+NEI] = 0;
    for (j = 0; j < MXOCUR; j++)
    {
      flag[j] = 0;
      prev[j] = NSA_ANT;
    }
    if (myfgets(propName, MAXN, lista))
    {
      propf = fopen(propName, "r");
      if (propf != NULL)
      {
        RELEVANCIA = RELEVANCIA_aux;
        prop = loadFile(propf, &Cp);
        if (testa_parametros(Cg, Cp))
          flagE += err_param(erros+NEI+i);
        else
        {
          n = matching(prop, prog, Cp.data_size, Cg.data_size, t, vr, 1, g);
          for (j = 0; j < n; j++)
          {
            if (t[j] < 0)
            {
              flag[j] = 1;
              t[j] = -t[j];
              if (argc == COM_ANTERIOR)
              {
                pn = matching(prop, pprog, MAX(t[j]/64, MINJ*2), Cpg.data_size, pt, pvr, 2, ga);
                if (pn<0)
                  prev[j] = ERRO_ANT;
                else
                  prev[j] = N_FIND_ANT;
                
                while (pn > 0)
                {
                  if(abs(64*Cpg.data_size-pt[pn-1]-t[j]) < TIME_DIF)
                  {
                    prev[j] = FIND_ANT;
                    vr[j] = MAX(vr[j], pvr[pn-1]);
                    pn = 0;
                  }
                  pn--;
                }
              }
            }
          }
          if (n > 0)
            RelGen(out, propName, progName, t, vr, flag, prev, g, n);
          else
            flagE += errmt(n, erros+NEI+i);
        }
        free(prop);
        fclose(propf);
      }
      else
        flagE += err2(ERR_OPN, erros+NEI+i);
    }
  }
  fclose(lista);
  fclose(out);
  return flagE;//<=4+#deProp
}