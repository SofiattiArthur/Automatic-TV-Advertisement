#include "wav.h"

/**
 * Extrai os coeficientes do áudio
 * @param filename nome do arquivo de áudio
 * @param samples amostras do áudio
 * @param header cabeçalho do arquivo de áudio
 * @param buffer buffer de memória
 * @return Retorna 0 se não houve erro
 * @return Retorna 1 se o nome do arquivo não foi expecificado
 * @return Retorna 2 se não conseguiu abrir o arquivo
 * @return Retorna 3 se o arquivo está quebrado
 * @return Retorna 4 se não é um arquivo .wav
 * @return Retorna 5 se não é formato PCM
 * @return Retorna 6 se houve erro no cabeçalho
 * @return Retorna 7 se houve erro na alocação da memória
 * @return Retorna 8 se arquivo das amostras está quebrado
 */
unsigned char wavread(char *filename, int16_t **samples, WavHeader **header, ChunkType **buffer)
{
  FILE *fp;
  //Se o nome do arquivo não foi expecificado
  if (!filename) 
    return (1);
  
  //Se não conseguiu abrir o arquivo
  fp = fopen(filename, "rb");
  if (fp == NULL) 
    return (2);
  
  //Se o arquivo está quebrado
  if (fread((*header), 1, sizeof(WavHeader), fp) != sizeof(WavHeader))
    return(3);
  
  //Se não é um arquivo .wav
  if (strncmp((*header)->chunk_id, "RIFF", 4) || strncmp((*header)->format, "WAVE", 4))
    return (4);
  
  //Se não é formato PCM
  if ((*header)->audio_format != 1)
    return (5);
  
  u_int8_t data = 0;
  //Lê cabeçalho
  do
  {
    if (fread((*buffer), 1, sizeof(ChunkType), fp) != sizeof(ChunkType)) 
      return (6); // Error chunk.
    
    if (!strncmp((*buffer)->chunk, "data", 4)) 
      data = 1;
    else 
      fseek(fp, (*buffer)->chunk_size, SEEK_CUR);
  }
  while(!data);

  //Lê as amostras
  if (*samples)
    free(*samples);
  
  *samples = (int16_t*) malloc((*buffer)->chunk_size);
  if (!*samples)
    return (7); // Error allocating memory.
  
  if (fread((*samples), 1, (*buffer)->chunk_size, fp) != (*buffer)->chunk_size) 
    return (8); //File broken: samples.
  
  fclose(fp);
  return(0);
}