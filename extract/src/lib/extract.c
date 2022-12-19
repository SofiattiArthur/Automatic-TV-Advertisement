/*  Exemplo de entrada:
 *  $ extracao /CAMINHO/PARA/ARQUIVO.wmv /CAMINHO2/PARA/DIRETORIO
 *
 *  $ extracao /CAMINHO/PARA/LISTA/ARQUIVOS.txt /CAMINHO2/PARA/LISTA/DIRETORIOS.mfc
 *
 *  Lista de Erros:
 *
 *  000: Arquivos de vídeo extraídos sem nenhum erro;
 *  001: Número de argumentos informados inválido, verifique a chamada da extração;
 *  002: Arquivo de vídeo é inválido, verifique sua extensão;
 *  003: Arquivo não encontrado,verifique se o nome está correto;
 *  004: Diretório não é reconhecido como um diretório, verifique se o mesmo é finalizado com o caractere “/”;
 *  005: Diretório não encontrado,verifique se o nome está correto;
 *  006: (Erro interno) Não foi possível converter o arquivo de vídeo, verifique se a FFmpeg está funcionando corretamente;
 *  007: (Erro interno) Nome do arquivo de áudio não especificado;
 *  008: (Erro interno) Não foi possível abrir o arquivo de áudio;
 *  009: (Erro interno) Arquivo de áudio quebrado;
 *  010: (Erro interno) Arquivo de áudio inválido;
 *  011: (Erro interno) Não foi possível codificar o PCM;
 *  012: (Erro interno) Cabeçalho do arquivo de áudio inválido;
 *  013: (Erro interno) Falha na alocação da memória dos coeficientes do arquivo de áudio;
 *  014: (Erro interno) Amostras do arquivo de áudio quebradas;
 *  015: (Erro interno) Não foi possível deletar o arquivo de áudio;
 *  016: (Erro interno) Falha ao criar arquivo “.mfc”;
 *  017: (Erro interno) Arquivo não possui diretório correspondente para salvar arquivo “.mfc”, verifique as listas informadas se o vídeo possui diretório correspondente;
 *  018: (Erro interno) Falha na alocação de memória da MFCC;
 *  019: (Erro interno) Falha na geração dos coeficientes da FFT;
 *  020: (Erro interno) Falha na geração dos coeficientes da DCT;
 *  021: (Erro interno) Falha no cálculo da FFT;
 *  022: (Erro interno) Falha no cálculo da DCT;
 *  023: Lista não possui extensão “.txt”, verifique os parâmetros de entrada;
 *  024: Arquivo de configurações possui variáveis não compatíveis;
 *
**/

#include <sys/stat.h>
#include <time.h>
#include <confuse.h>
#include "funcoes.h"
#include "converte.h"
#include "wav.h"
#include "mfcc.h"

//Cabeçalho para arquivo binário ".mfc"
typedef struct {
    u_int8_t mfcc[4];       /**< string "MFCC" marca o inicio do arquivo (4 bytes)*/
    u_int16_t n_fft;        /**< numero de pontos utilizados em cada janela para calcular a fft (2 bytes)*/
    u_int16_t step;         /**< tamanho do intervalo entre duas janelas (2 bytes)*/
    u_int16_t n_coef;       /**< numero de coeficientes gerados por janela (2 bytes)*/
    u_int16_t comp;         /**< numero de coeficientes salvos no arquivo por janela (2 bytes)*/
    u_int32_t data_size;    /**< numero de bytes ate o final do arquivo (4 bytes)*/
} cabecalho;

//Variáveis do arquivo de configuracoes
#define CONF_PARAM      "-c"
#define CONFNAME        "extraction.conf" //nome do arquivo de configuracao
#define EXT_1_DEF       ".wmv"
#define EXT_2_DEF       ".asf"
#define EXT_3_DEF       ".mp4"
#define EXT_4_DEF       "none"
#define EXT_5_DEF       "none"
#define STP_DEF         512
#define FFT_DEF         1024
#define NCO_DEF         20
#define CMP_DEF         13
char EXT_1[5];
char EXT_2[5];
char EXT_3[5];
char EXT_4[5];
char EXT_5[5];
u_int16_t STP;
u_int16_t FFT;
u_int8_t NCO;
u_int16_t CMP;

//******************************************ARQUIVO DE CONFIGURAÇÕES**************************************************//

/**
 * Testa os parâmetros recebidos de um arquivo de configuração
 * @return Retona 2^n, caso o enésimo termo em ordem de aparição no
 * @return arquivo .conf esteja errado, caso mais de um termo esteja
 * @return errado o retorno é o somatório de 2^a + 2^b +..., caso não
 * @return haja nenhum erro, retorna 0
 */
int testa_conf()
{
  int flag = 0, aux;

  if ((intervalo(FFT, 512, 2048)) || !(FFT && !(FFT & (FFT - 1)))) 
  {
    FFT = FFT_DEF;
    flag = 1;
  }

  if ((intervalo(STP, 128, (FFT/2))) || !(STP && !(STP & (STP - 1)))) 
  {
    STP = FFT/2;
    flag += (1<<1);
  }
  
  if (intervalo(NCO, 8, 32)) 
  {
    NCO = NCO_DEF;
    flag += (1<<2);
  }
  
  if (intervalo(CMP, 8, 32)) 
  {
    CMP = CMP_DEF;
    flag += (1<<3);
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
  int16_t aux;
  FILE *conf;

  sprintf(EXT_1, "%s", EXT_1_DEF);
  sprintf(EXT_2, "%s", EXT_2_DEF);
  sprintf(EXT_3, "%s", EXT_3_DEF);
  sprintf(EXT_4, "%s", EXT_4_DEF);
  sprintf(EXT_5, "%s", EXT_5_DEF);
  STP = STP_DEF;
  FFT = FFT_DEF;
  NCO = NCO_DEF;
  CMP = CMP_DEF;

  conf = fopen(CONFNAME, "w");
  if (conf == NULL) 
    return 1;

  fprintf(conf, "#EXTENSOES PARA ARQUIVOS DE VIDEO\n");
  fprintf(conf, "EXT_1 = %s\n", EXT_1_DEF);
  fprintf(conf, "EXT_2 = %s\n", EXT_2_DEF);
  fprintf(conf, "EXT_3 = %s\n", EXT_3_DEF);
  fprintf(conf, "EXT_4 = %s\n", EXT_4_DEF);
  fprintf(conf, "EXT_5 = %s\n\n", EXT_5_DEF);
  fprintf(conf, "#NUMERO DE JANELAS (OBS.: O VALOR DEVE ESTAR ENTRE 128 E A METADE DA FFT E SER POTENCIA DE 2)\n");
  fprintf(conf, "STP = %d\n", STP_DEF);
  fprintf(conf, "#TAMANHO DA FFT (OBS.: O VALOR DEVE ESTAR ENTRE 512 E 2048 E SER POTENCIA DE 2)\n");
  fprintf(conf, "FFT = %d\n", FFT_DEF);
  fprintf(conf, "#NUMERO DE COEFICIENTES (OBS.: O VALOR DEVE ESTAR ENTRE 8 E 32)\n");
  fprintf(conf, "NCO = %d\n", NCO_DEF);
  fprintf(conf, "#NUMERO DE COEFICIENTES PARA SALVAR NO ARQUIVO '.mfc' (OBS.: O VALOR DEVE ESTAR ENTRE 8 E 32)\n");
  fprintf(conf, "CMP = %d\n", CMP_DEF);

  fclose(conf);
  return 0;
}

/**
 * Define novo arquivo de configurações
 * @param name nome do arquivo de configurações
 * @return 0 - caso não ocorra erro
 * @return 1 - caso algum dos parâmetros do arquivo de configuração esteja errado
 * @return 2 - caso ocorra o erro 1 e não seja possível criar um novo arquivo
 * @return 3 - caso não seja possível abrir o arquivo de configuração
 * @return 4 - caso ocorra o erro 3 e não seja possível criar um novo arquivo
*/
int load_conf(char* name)
{
  int flag;
  int16_t aux;
  cfg_t *cfg;

  cfg_opt_t opts[] = {
    CFG_STR("EXT_1", EXT_1_DEF, CFGF_NONE),
    CFG_STR("EXT_2", EXT_2_DEF, CFGF_NONE),
    CFG_STR("EXT_3", EXT_3_DEF, CFGF_NONE),
    CFG_STR("EXT_4", EXT_4_DEF, CFGF_NONE),
    CFG_STR("EXT_5", EXT_5_DEF, CFGF_NONE),
    CFG_INT("STP", STP_DEF, CFGF_NONE),
    CFG_INT("FFT", FFT_DEF, CFGF_NONE),
    CFG_INT("NCO", NCO_DEF, CFGF_NONE),
    CFG_INT("CMP", CMP_DEF, CFGF_NONE),

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

  sprintf(EXT_1, "%s", cfg_getstr(cfg, "EXT_1"));
  sprintf(EXT_2, "%s", cfg_getstr(cfg, "EXT_2"));
  sprintf(EXT_3, "%s", cfg_getstr(cfg, "EXT_3"));
  sprintf(EXT_4, "%s", cfg_getstr(cfg, "EXT_4"));
  sprintf(EXT_5, "%s", cfg_getstr(cfg, "EXT_5"));
  STP = cfg_getint(cfg, "STP");
  FFT = cfg_getint(cfg, "FFT");
  NCO = cfg_getint(cfg, "NCO");
  CMP = cfg_getint(cfg, "CMP");

  cfg_free(cfg);
  return 0;
}

/**
 * Localiza se o arquivo de configuração nos parâmetros de entrada
 * @param argc número de argumentos de entrada
 * @param argv argumentos de entrada
*/
int conf_par(int argc, char** argv)
{
  int i;
  for (i = 0; i < (argc - 1); i++)
  {
    if (!strcmp(CONF_PARAM, argv[i])) 
      return (i);
  }
  return (9);
}

/**
 * Coloca parâmetros do arquivo de configuração para o final dos parâmtros de entrada
 * @param argc número de argumentos de entrada
 * @param argv argumentos de entrada
 * @param p posição do argumento '-c'
*/
void desloca_argv(int argc, char **argv, int p)
{
  int i;
  for (i = p+2; i < argc; i++)
      argv[i-2] = argv[i];
}

//***************************************************VALIDAÇÕES*******************************************************//

/**
 * Valida arquivo de vídeo
 * @param arq nome do arquivo de vídeo
 * @return 0 - caso não ocorra erro
 * @return 2 - caso não possua extensão válida
 * @return 3 - caso arquivo não exista
*/
char valida_arquivo(char *arq)
{
  //Testa se o arquivo tem extensao válida
  if( ((posicao(arq, EXT_1)) == 0) && (EXT_1 != "none") &&
      ((posicao(arq, EXT_2)) == 0) && (EXT_2 != "none") &&
      ((posicao(arq, EXT_3)) == 0) && (EXT_3 != "none") &&
      ((posicao(arq, EXT_4)) == 0) && (EXT_4 != "none") &&
      ((posicao(arq, EXT_5)) == 0) && (EXT_5 != "none")) 
  {
    return(2);
  }

  //Testa se arquivo existe
  if (!existe(arq))
    return(3);

  return(0);
}

/**
 * Valida diretório
 * @param dir nome do diretório
 * @return 0 - caso não ocorra erro
 * @return 4 - caso não seja diretório
 * @return 5 - caso diretório não exista
*/
char valida_diretorio(char *dir) 
{
  //Testa se e diretorio
  if (dir[strlen(dir) - 1] != '/')
    return(4);
  
  //Testa se diretorio existe
  if(!existe(dir))
    return(5);
  
  return(0);
}

//***************************************************ESTRUTURA********************************************************//

/**
 * Valida, converte, abre e extrai coeficientes
 * @param arq arquivo de vídeo
 * @param dir diretório para salvar o .mfc
 * @return Retorna o status do arquivo
*/
char prepara_arquivo(char *arq, char *dir) 
{
  int err, a, b;
  //________________________________________________________________________________________________________________//
  //Valida arquivo
  err = valida_arquivo(arq);
  if(err) 
    return(err);
  
  err = valida_diretorio(dir);
  if(err)
    return(err);
  
  //________________________________________________________________________________________________________________//
  //Converte o arquivo
  if(converte(arq) != 0)
    return(6);
  
  //________________________________________________________________________________________________________________//
  //Extrai os coeficientes
  int16_t *samples = NULL;
  WavHeader *header;
  header = (WavHeader*)malloc(sizeof(WavHeader));
  ChunkType *buffer;
  buffer = (ChunkType*)malloc(sizeof(ChunkType));
  err = wavread(arq, &samples, &header, &buffer);
  if(err != 0) 
  {
    remove(arq);
    return(err + 6);
  }
  
  unsigned int nsamples = (8 * buffer->chunk_size) / (header->num_channels * header->bps);
  //Normaliza as amostras
  float *nrm, *dad;
  nrm = malloc(sizeof(float)*nsamples);
  dad = nrm;
  for(a = 0; a < nsamples*header->num_channels; a = a + header->num_channels)
  {
    (*nrm) = samples[a] / (float) 32760;
    nrm++;
  }
  nrm=dad;
  //________________________________________________________________________________________________________________//
  //Deleta arquivo ".wav"
  if((remove(arq)) != 0)
    return(15);
  
  //________________________________________________________________________________________________________________//
  //Calcula os coeficientes da MFCC
  float **mat;
  int s;
  s = ceil((float)(nsamples-FFT)/STP);
  mat = malloc(sizeof(float*)*s);
  for (a=0;   a<s;    a++)
    mat[a]=malloc(sizeof(float)*NCO);
  
  err = mfcc(mat, FFT ,STP ,NCO , nrm, header->sample_rate, nsamples);
  if(err != 0)
    return((err*-1)+17);
  
  //________________________________________________________________________________________________________________//
  //Salva arquivo ".mfc"
  nome_arquivo(arq, dir);
  muda_extensao(dir, posicao(dir, ".wav"), ".mfc");
  FILE *mfc;
  if ((mfc = fopen(dir, "wb")) == NULL)
    return(16);
  
  //Configura cabeçalho
  cabecalho inf;
  int16_t **data;
  inf.mfcc[0] = 'M';
  inf.mfcc[1] = 'F';
  inf.mfcc[2] = 'C';
  inf.mfcc[3] = 'C';
  inf.n_fft = FFT;
  inf.step = STP;
  inf.n_coef = NCO;
  inf.comp = CMP;
  inf.data_size = s * NCO * 2;
  fwrite(&inf, 1, sizeof(inf), mfc);
  data = malloc(sizeof(int16_t*)*s);
  for(a = 0 ; a < s ; a++)
  {
    data[a] = malloc(sizeof(int16_t)*NCO);
    for(b = 0 ; b < CMP ; b++)
    {
      data[a][b] = mat[a][b] * 256;
      fwrite(&(data[a][b]), 1, sizeof(int16_t), mfc);
    }
    free(mat[a]);
    free(data[a]);
  }
  fclose(mfc);
  //________________________________________________________________________________________________________________//
  free(samples);
  free(header);
  free(buffer);
  free(nrm);
  free(mat);
  free(data);
  return(0);
}

//*****************************************************INICIO*********************************************************//

/**
 * Chama as funções do módulo de extração
 * @param argc número de argumentos enviados
 * @param argv argumentos de entrada
 * @param erro vetor para armazenar o erros e status
 * @return Retorna o número de posições utilizadas do vetor 'erro'
*/
int extracao(int argc, char *argv[], char erro[])
{
  char arqori[128];//Nome do arquivo original
  char dirori[128];//Nome do diretório original
  int i = 0, p = 0;
  char confName[128];
  //________________________________________________________________________________________________________________//
  //Carrega arquivo de configurações
  i = conf_par(argc, argv);
  if (i != 9)
  {
    strcpy(confName, argv[i+1]);
    desloca_argv(argc, argv, i);
    argc -= 2;
  }
  else
    strcpy(confName, CONFNAME);
  
  if ((load_conf(confName)) != 0) 
    new_conf(confName);
  else if ((i = testa_conf()))
  {
    new_conf(confName);
    p = 1;
    erro[0] = 24;
    return(p);
  }

  //________________________________________________________________________________________________________________//
  //Se é arquivo
  if(strncmp(argv[0], "-l", 2))
  {
    //Se não foram informados 3 argumentos
    if(argc != 2)
    {
      p = 1;
      erro[0] = 1;
      return(p);
    }
    //Valida, converte, abre e extrai coeficientes
    strcpy(arqori, argv[0]);
    strcpy(dirori, argv[1]);
    i = prepara_arquivo(argv[0], argv[1]);
    if(i)
    {
      p = 1;
      erro[0] = i;
      return(p);
    }
    else
    {
      p = 1;
      erro[0] = 0;
      return(p);
    }
  }
  //________________________________________________________________________________________________________________//
  //Se é lista
  else 
  {
    //Se não foram informados 4 argumentos
    if(argc != 3)
    {
      p = 1;
      erro[0] = 1;
      return(p);
    }
    //Testa se listas são válidas
    if(!existe(argv[1]))
    {
      p = 1;
      erro[0] = 3;
      return(p);
    }
    if((posicao(argv[1], ".txt")) == 0)
    {
      p = 1;
      erro[0] = 23;
      return(p);
    }
    if(!existe(argv[2]))
    {
      p = 1;
      erro[0] = 3;
      return(p);
    }
    if((posicao(argv[2], ".txt")) == 0)
    {
      p = 1;
      erro[0] = 23;
      return(p);
    }
    //Lê linhas da lista de arquivo e de diretório
    FILE *la, *ld;
    char arq[128], dir[128];
    la = fopen (argv[1],"r");
    ld = fopen (argv[2],"r");
    if ((la != NULL) && (ld != NULL))
    {
      while((fgets(arq, sizeof(arq), la))!=NULL) 
      {
        if ((fgets(dir, sizeof(dir), ld))!=NULL) 
        {
          if (feof(la) == 0) 
            arq[strlen(arq) - 1] = '\0';

          if (feof(ld) == 0) 
            dir[strlen(dir) - 1] = '\0';

          //Valida, converte, abre e extrai coeficientes
          strcpy(arqori, arq);
          strcpy(dirori, dir);
          i = prepara_arquivo(arq, dir);
          p++;
          erro[p-1] = i;
        }
        else
        {
          p++;
          erro[p-1] = 17;
        }
      }
      fclose(la);
      fclose(ld);
    }
  }
  return(p);
}