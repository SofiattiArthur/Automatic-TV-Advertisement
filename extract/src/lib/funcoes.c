#include <stdlib.h>
#include "funcoes.h"

/**
 * Localiza conjunto de caracteres na string
 * @param str string
 * @paramext conjunto de caracteres a localizar
 * @return Retorna '0' para não encontrado ou a posição do conjunto de caracteres na string
*/
unsigned int posicao(char *str, char *ext)
{
  char *aux;
  int pos;
  aux = stristr(str, ext);
  if (aux == NULL) 
    return(0);
  
  pos = (int) (aux - str);
  return(pos);
}

/**
 * Verifica se arquivo/diretório existe
 * @param str nome do arquivo/diretório
 * @return Retorna '0' se não existe e '1' se existe
*/
unsigned char existe(char *str) 
{
  struct dirent info;
  int ax1, ax2 = 0;
  if (str[strlen(str) - 1] == '/') 
    ax2 = 1;

  if (ax2 == 1)
    str[strlen(str) - 1] = '\0';

  ax1 = stat(str, &info);
  
  if(ax2 == 1) 
    str[strlen(str)] = '/';

  if (ax1 < 0) 
    return(0);

  return(1);
}

/**
 * Modifica o diretório do arquivo
 * @param arq nome do arquivo com antigo diretório
 * @param mfc nome do novo diretório
 * @return Retorna na variável mfc o nome do arquivo com o novo diretório
*/
void nome_arquivo(char *arq, char *mfc) 
{
  int var, tam, aux;
  char nom[128];
  for (var = strlen(arq); var > 0; var--)
  {
    if(arq[var] == '/') 
      break;
  }

  for (aux = var; aux <= strlen(arq); aux++) 
    nom[aux - var] = arq[aux];

  strcat(mfc, nom);
}

/**
 * Modifica a extensão do arquivo
 * @param arq arquivo para modificar a extensão
 * @param pos posição da extensão
 * @param ext nova extensão
 * @return Retorna na variável arq o nome do arquivo com a nova extensão
*/
void muda_extensao(char *arq, int pos, char *ext)
{
  int aux;
  for (aux = 0; aux < strlen(ext); aux++) 
    arq[pos + aux] = ext[aux];
}

/**
 * Localiza a posição do caracter na string
 * @param st1 string
 * @param st2 caracter
 * @return Retorna a posição do caracter na string ou 'NULL' se não encontrar
*/
char* stristr(char* st1, const char* st2) 
{
  char* ps1 = st1 ;
  const char* ps2 = st2 ;
  char* aux = *ps2 == 0 ? st1 : 0;
  while (*ps1 != 0 && *ps2 != 0) 
  {
    if(tolower(*ps1) == tolower(*ps2))
    {
      if(aux == 0)
        aux = ps1;

      ps2++ ;
    }
    else 
    {
      ps2 = st2;
      if (tolower(*ps1) == tolower(*ps2))
      {
        aux = ps1;
        ps2++;
      }
      else
        aux = 0;
    }
    ps1++;
  }

  return (*ps2 == 0 ? aux : 0);
}

/**
 * Verifica se o valor pertence ao intervalo indicado
 * @paramvalor = valor para verificação
 * li = limite inferior
 * ls = limite superior
 * @return Retorna '0' se o valor está dentro do intervalo e '1' se não
 */
unsigned char intervalo(int valor, int li, int ls)
{
  if (valor >= li && valor <= ls) 
    return 0;

  return 1;
}