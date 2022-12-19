#include "mfcc.h"

/**
 *Cria janela de Hanning de tamanho L
 * @param x vetor de saida com os termos da janela
 * @param L tamanho da janela
*/
void hann(float *x, unsigned int L)
{
	int i;
	int N = L-1;
	for (i = 0; i <= N; i++)
		x[i] = 0.5*(1-cos(2*M_PI*i/N));
}

/**
 * Calcula os coeficientes da MFCC de um vetor de entrada dividido em janelas de tamanho fixo
 * Tamanho da matriz de saida:
 *  fo[n][m];
 *  n=ceil((float)(s-nfft)/step);
 *  m=ncoef;
 *
 * @param fo matriz de coeficientes MFCC de saida
 * @param nfft numero de pontos da FFT
 * @param step distancia entre duas janelas
 * @param ncoef numero de coeficientes de cada janela
 * @param x vetor com dados de entrada
 * @param fs frequencia de amostragem dos dados de entrada
 * @param s tamanho do vetor x
 * @return Retorna 0 caso tudo corra bem ou outro valor de acordo com o erro que ocorreu:
 * @return -1 caso haja erro na alocação de memória
 * @return -2 caso haja erro na geração dos coeficientes da FFT
 * @return -3 caso haja erro na geração dos coeficientes da DCT
 * @return -4 caso haja erro no cálculo da FFT
 * @return -5 caso haja erro no cálculo da DCT
 */
int mfcc(float ** fo, unsigned int nfft, unsigned int step, unsigned int ncoef, float *x, unsigned int fs, unsigned int s)
{
	unsigned int *fi, *fin, nc, N1;
	float **flt, *xx, *temp, *fm, *fb, *hn, *lf, *pw, *W;
	float f1, fm1, fm2, dfm;
	long int i;
	int j, k, m, n, aux;

	//filtragem de pre-emphasis
	for (i = 0; i < (s-1); i++)
		x[i] = (float)x[i+1]-0.97*x[i];

	//mel scale filter bank
	nc = ncoef;
	f1 = 200;
	fm1 = 1125*log(1+f1/700.0);
	fm2 = 1125*log(1+fs/1400.0);
	dfm = (float)(fm2-fm1)/(nc+1.0);

	//cria o vetor de pontos centrais dos filtros triangulares na escala de mel
	fm = malloc(sizeof(float)*(nc+2));
	if (fm == NULL)
		return -1;

	fb = malloc(sizeof(float)*(nc+2));

	if (fb == NULL) 
		return -1;

	fi = malloc(sizeof(unsigned int)*(nc+2));
	if(fi == NULL)
		return -1;
	
	for (k = 0; k < (nc+2); k++)
	{
		fm[k] = fm1 + k*dfm;
		fb[k] = 700*(exp(fm[k]/1125.0)-1); 	//filterbank frequency
		fi[k] = ((nfft*fb[k])/fs); 			//indices das frequencias
	}
	
	flt = malloc(sizeof(float*)*nc);
	if (flt == NULL)
		return -1;

	for (i = 0; i < nc; i++){
		flt[i] = malloc(sizeof(float)*(fi[nc+1]-fi[nc-1]));
		if(flt[i] == NULL) 
			return -1;
	}

	// cria os pesos do banco de filtros
	fin = malloc(sizeof(unsigned int)*nc);
	if(fin == NULL) 
		return -1;
	
	for (i = 0; i < nc; i++)
	{
		fin[i] = fi[i+2]-fi[i];
		k = 0;
		for (j = fi[i]; j <= (fi[i+2]); j++)
		{
			if (j <= fi[i+1])
				flt[i][k] = (float)1.0*(j-fi[i])/(fi[i+1]-fi[i]);
			else
				flt[i][k] = (float)1.0*(fi[i+2]-j)/(fi[i+2]-fi[i+1]);
			
			k++;
		}
	}

	N1 = nfft;
	m = 0;

	n = ceil((float)(s-nfft)/step);

	hn = malloc(sizeof(float)*nfft);
	if (hn == NULL)
		return -1;
	
	hann(hn, nfft);	//cria uma janela de hanning de tamanho nfft
	
	//liftering 
	lf = malloc(sizeof(float)*nc);
	if (lf == NULL) 
		return -1;
	
	for (k = 0; k < nc; k++)
		lf[k] = 1+sin(M_PI*(k+1)/nc)*nc/2;
	
	pw = coef_wk(N1);			//expoentes para FFT
	if (pw == NULL) 
		return -2;

	W = W_coef(nc);			//expoentes para DCT
	if (W == NULL) 
		return -3;
	
	temp = malloc(sizeof(float)*(N1/2+1));
	if (temp == NULL) 
		return -1;
	
	xx = malloc(sizeof(float)*nfft);
	if (xx == NULL) 
		return -1;
	
	for (aux = 0; aux < (s-nfft); aux += step)
	{
		for (i = aux; i < (aux+N1); i++)
			xx[i-aux]=(double)hn[i-aux]*x[i];
		
		if (fft(N1, xx, temp, pw) == -1) 
			return -4;

		for (i = 0; i < (nfft/2); i++)
			temp[i] *= temp[i];
		
		for (j=0;	j<nc;	j++)
		{
			fo[m][j] = 0;
			for (k = 0; k < fin[j]; k++)
				fo[m][j] += flt[j][k]*temp[k+fi[j]-1];
			
			fo[m][j] = log10(fo[m][j] + 0.0001);
		}

		if (dct(nc, fo[m], fo[m], W) == -1) 
			return -5;

		m++;
	}
	
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < nc; j++)
			fo[i][j] *= lf[j];
	}
	//libera memoria alocada
	for (i = 0; i < nc; i++)
		free(flt[i]);
	
	free(fm);
	free(fb);
	free(fi);
	free(flt);
	free(fin);
	free(hn);
	free(lf);
	free(pw);
	free(W);
	free(temp);
	free(xx);
	return 0;
}
