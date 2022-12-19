#ifndef WAV_H
#define WAV_H

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>

// Formato do cabeçalho do arquivo wave
typedef struct
{
  u_int8_t chunk_id[4];     /**< Marca o arquivo como um arquivo TIFF */
  u_int32_t chunk_size;     /**< Tamanho geral do arquivo (32-bit inteiro) */
  u_int8_t format[4];       /**< Tipo de arquivo de cabeçalho */
  u_int8_t fmtchunk_id[4];  /**< Formato do marcador do cabeçalho */
  u_int32_t fmtchunk_size;  /**< Comprimento dos dados de formato */
  u_int16_t audio_format;   /**< Tipo de formato (1 é PCM) – 2 byte inteiro */
  u_int16_t num_channels;   /**< Numero de canais – 2 byte inteiro */
  u_int32_t sample_rate;    /**< Taxa de amostragem (32-bit inteiro). Numero de amostras por segundo, ou Hertz */
  u_int32_t byte_rate;      /**< (Taxa de amostragem * Bits por amostra * Canais) / 8 */
  u_int16_t block_align;    /**< (Bits por amostra * Canais) / 8 */
  u_int16_t bps;            /**< Bits por amostra */
} WavHeader;

typedef struct 
{
  u_int8_t chunk[4];    /**< Cabeçalho */
  u_int32_t chunk_size; /**< Tamanho do cabeçalho */
} ChunkType;

unsigned char wavread(char *filename, int16_t **samples, WavHeader **header, ChunkType **buffer);

#endif //WAV_H
