#include "converte.h"

/**
 * Extrai o áudio do arquivo de vídeo
 * @param arq nome do arquivo de vídeo
 * @return Retorna '0' para seucesso e '1' para erro
*/
char converte(char *arq)
{
  char com[306]; //Ponteiro para comando para extrair audio do video
  char wmv[128];
  int pos, err;
  
  //Modifica extensão
  strcpy(wmv, arq);
  pos = posicao(arq, ".wmv");
  if(pos == 0) pos = posicao(arq, ".asf");
  if(pos == 0) pos = posicao(arq, ".mp4");
  muda_extensao(arq, pos, ".wav");
  
  // Copia variáveis para extrair o audio
  strcpy(com, "ffmpeg -y -i ");
  strcat(com, wmv);
  strcat(com, " -ar 8000 -acodec pcm_s16le ");
  strcat(com, arq);
  
  //Extrai apenas o audio do arquivo
  err = system(com);
  if(err != 0)
  {
    strcpy(arq, wmv);
    return(1);
  }
  
  return(0);
}