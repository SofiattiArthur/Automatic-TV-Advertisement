#include "FFT.h"

/**
 * Calcula os coeficientes wk para fft de n pontos
 * @param n numero de termos da fft
 * @return Retorna os coeficientes em um ponteiro de float
 */

float* coef_wk(unsigned int n)
{
	unsigned int i;
	float *pw, *pw0;
	double q=0,k;
	
	pw = malloc(sizeof(float)*n);
	if (pw == NULL) 
	  return NULL;
	
	pw0 = pw;
	
	k = 2*M_PI/n;
	for (i = 0; i < n/2; i++)
	{
		*pw = (float)cos(q);
		pw++;
		*pw = (float)-sin(q);
		pw++;
		q+=k;
	}
	
	return pw0;
}


/**
 * Calcula reversao de bits para palavra i de l bits
 * @param i palavra a ser revertida
 * @param l numero de bits da palavra
 * @return Retorna i em bit reverso
 */

int btrv(int i,int l)
{
	int j, a = 0;

	for (j = l-1; j >= 0; j--)
	{
		a |= (1&i)<<j;
		i >>= 1;
	}

	return a;
}


/**
 * Coloca os dados de entrada na sequencia de bit reverso
 * @param n numero de pontos
 * @param ptrt ponteiro para os dados de entrada
 * @param ptrf ponteiro para os dados de saida, retorna os dados
 * reordenados e com zeros entre um dado e outro
 */
void bitrev(int n,float *ptrt,float *ptrf)
{
	int l, i, n0;
	
	n0 = n;
	for (l = 0; n0 > 1; l++, n0 >>= 1);
	
	for (i = 0; i < n; i++)
	{
		*ptrf++=*(ptrt+btrv(i,l));
		*ptrf++=0;
	}
}

/**
 * Ver <OPPENHEINM, Alan V.; DISCRETE-TIME SIGNAL PROCESSING. p593>
 * calcula transformada rapida de furier
 * @param n numero de pontos da transformada
 * @param ptrt ponteiro para os dados de entrada
 * @param ptrf ponteiro para os dados de saida
 * @param ptrw ponteiro para os coeficientes wk da fft, coloca em ptrf o módulo dos n+1 primeiros termos da transformada
 * @return Retorna 1 caso tudo corra bem
 * @return Retorna -1 em caso de erro
 */
int fft(unsigned int n,float *ptrt,float *ptrf,float *ptrw)
{
	unsigned int i,estg;
	int p,tp,b,w,j,nb;
	float re,im;				//Real e Imaginário
	double aux;
	float *ptrw0,*ptr0, *ptr;
	
	//Alocação de memória
	ptr = (float *)calloc(2*n,sizeof(float));
	if (ptr == NULL)
	 	return -1;
	
	//Guarda endereço inicial dos ponteiros
	ptr0=ptr;
	ptrw0=ptrw;
	
	//Reverte os itens do vetor de entrada
	bitrev(n,ptrt,ptr);
		
//	estg=(int)ceil((log(n)/log(2)));		//estg=log2(n) ex.:n=1024 -> estg=10
	i=n;
	for (estg = 0; i > 1; estg++, i >>= 1);
	
	for (i = 1; i <= estg; i++)
	{
		b = 1<<i;
		tp = (int)n/b;						//tp=n/(2^i)
		w = tp<<1;								//w=2*n/(2^i)
		nb = b>>1;								//nb=(2^i)/2

		for(p = 1; p <= tp; p++)
		{
			ptrw = ptrw0;		//ptrw volta a posição inicial
			for(j = 0; j < nb; j++)
			{
				ptr += b;
				re = (*(ptr))*(*(ptrw));	//re=ptr[b*p+2*j]*ptrw[w*j]
				ptr++;
				ptrw++;
				re -= (*ptr)*(*(ptrw));		//re-=ptr[b*p+1+2*j]*ptrw[1+w*j]
				ptrw--;
				im = (*(ptr))*(*(ptrw));	//im=ptr[b*p+1+2*j]*ptrw[w*j]
				ptr--;
				ptrw++;
				im += (*ptr)*(*(ptrw));		//im+=ptr[b*p+2*j]*ptrw[1+w*j]
				ptrw--;
				*ptr = (*(ptr-b)-re)/2;		//ptr[b*p+2*j]=(ptr[(b*(p-1)+2*j]-re)/2
				ptr++;
				*ptr = (*(ptr-b)-im)/2;		//ptr[b*p+1+2*j]=(ptr[b*(p-1)+1+2*j]-im)/2
				ptr -= b;
				*ptr = (*(ptr)+im)/2;			//ptr[b*(p-1)+1+2*j]=(ptr[b*(p-1)+1+2*j]+im)/2
				ptr--;
				*ptr = (*ptr+re)/2;				//ptr[b*(p-1)+2*j]=(ptr[b*(p-1)+2*j]+re)/2

				ptr += 2;
				ptrw += w;
			}
			ptr += b;
		}
		ptr = ptr0;		//ptr volta a posição inicial
	}

	for (i = 0; i <= n/2; i++)
	{
		aux = (*ptr)*(*ptr);
		ptr++;
		*ptrf = (float)n*sqrt(aux+(*ptr)*(*ptr));
		ptr++;
		ptrf++;
	}
	
	free((float *)ptr0);
	return 1;
}
