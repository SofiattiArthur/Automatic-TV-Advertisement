#include "DCT.h"

#define M_R2			1.4142135623731
#define M_R2_2		0.707106781186548

/**
 * Coloca os dados de entrada na sequencia de bit reverso
 * @param n numero de pontos
 * @param ptrt ponteiro para os dados de entrada
 * @param ptrf ponteiro para os dados de saida, retorna os dados reordenados e sem zeros entre um dado e outro
 */
void bitrev2(int n,float *ptrt,float *ptrf)
{
	int l,i,n0;
	
	n0=n;
	for (l = 0; n0 > 1; l++, n0 >>= 1);
	
	for (i = 0; i < n; i++)
	{
		*ptrf = *(ptrt+btrv(i,l));
		ptrf++;
	}
}

/**
 * Calcula os senos e cossenos para dct de N pontos
 * @param N numero de pontos da DCT
 * @return Retorna os coeficientes em um vetor de float
 */
float* W_coef(unsigned int N)
{
	int i, j;
	unsigned int l;
	float ang0, ang1;
	float *W, *w0;

	for (i = (N>>1), N = 0; i > 1; i >>= 1)
	{
		for (j = i; j > 1; j >>= 1)
			N += j;
	}

	W = malloc(sizeof(float)*2*N);
	if (W == NULL)
		return NULL;
	
	w0 = W;

	ang0 = (float)M_PI/4;
	l = 1;
	for (i = 2; i <= N; i <<= 1)
	{
		for (j = (i>>1); j < (i); j++)
		{
			ang1 = btrv(j,l)*ang0;
			(*W) = cos(ang1);
			W++;
			(*W) = sin(ang1);
			W++;
		}
		ang0 /= 2;
		l++;
	}
	
	return w0;
}

/**
 * Calcula a transformada literal do cosseno em tempo discreto
 * @param N numero de pontos da DCT
 * @param x vetor de pontos de entrada
 * @param ans vetor de pontos de saida
 * @return Retorna 1
 */
int litDCT(unsigned int N, float *x, float *ans)
{
	int i, j, N2;
	float mult;

	mult = 1/(sqrt(N));

	N2 = 2*N;

	ans[0] = 0;
	for (j = 0; j < N; j++)
			ans[0]+=x[j];
	
	ans[0] *= mult;

	mult *= M_R2;

	for (i = 1; i < N; i++)
	{
		ans[i] = 0;
		for (j = 0; j < N; j++)
				ans[i] += x[j]*cos(M_PI*(2*j+1)*i/N2);
		
		ans[i] *= mult;
	}

	return 1;
}

/**
 * Calcula transformada rapida do cosseno em tempo discreto
 * @param N numero de pontos da transformada
 * @param x ponteiro para os dados de entrada
 * @param y ponteiro para os dados de saida
 * @param W ponteiro para os senos e cossenos da dct
 * @return Retorna 1 caso tudo corra bem
 * @return Retorna -1 em caso de erro
 */
int dct(unsigned int N, float *x, float *y, float *W)
{
	int i, j, k, l, rv, s;
	int c;
	unsigned int n, N_2, v1, v2, Mn, N0;
	float *y0, *w0, *yaux;
	float aux;

	//Inicia parâmetros
	y0=y;
	w0=W;
	N0=N;
	n=N;
	N_2=N>>1;

    i=(int)log(N)/log(2);

	yaux=malloc(sizeof(float)*N);
	if (yaux == NULL)
		return -1;

    if (i != N)
		{
      c = litDCT(N, x, yaux);
			for (i = 0; i < N; i++)
				y[i] = yaux[i];
		
			return c;
    }
	
	//y=x
	for (i = 0; i < N; i++)
	{
		*y = *x;
		y++;
		x++;
	}
	y=y0;
	
	if (N == 1)
	{
		free(yaux);
		return 1;
	}
	else if (N == 2)
	{
		aux = (y[0]+y[1])*M_R2_2;
		y[1] = (y[0]-y[1])*M_R2_2;
		y[0] = aux;
		
		free(yaux);
		return 1;
	}
	
	while(1),
	{
		//1 - Butterflies
		for (i = 0; i < (N_2); i++){
			v2 = (N-1)-i;
			
			aux = y[i] + y[v2];
			y[v2] = y[i] - y[v2];	
			y[i] = aux;			
		}
		
		//2 - divide o vetor em dois, a primeira metade calcula dct com N/2 elementos com recursividade, a segunda segue o diagrama de Chen. Se N = 4, calcula explicitamente a dct desse vetor
		if (N > 4)
		{
			y+=N_2;	
			n>>=1;
		}
		else if (N == 4)
		{
			aux = (float)(y[0]+y[1])*M_R2_2;
			y[1] = (float)(y[0]-y[1])*M_R2_2;
			y[0] = aux;

			aux = (float)y[2]*(*(W+3))+y[3]*(*(W+2));
			y[3] = (float)y[3]*(*(W+4))-y[2]*(*(W+5));
			y[2] = aux;
			
			bitrev2(N0,y0,yaux);
			
			aux = (float)M_R2/sqrt(N0);
			for(i = 0; i < N0; i++)
			{
				*y0 = (*yaux)*aux;
				y0++;
				yaux++;
			}
			
			free(yaux-N0);
			return 1;
		}
		else
		{
			free(yaux);
			return -1;
		}
	
		//3 - Multiplicação pelos senos e cossenos e Butterflies das metades
		for (i = 1; (N_2>>i) > 1; i++)
		{
			//Multiplicação pelos cossenos e senos
			c = (1<<i)-2;							//c começa em 0 
			Mn = N_2/(n);
			for	(j = 0; j < (Mn>>1) || j == 0; j++)
			{
				s=1;
				for	(k = 0; k < (n>>1) && ((i!=1) || (k < (n>>2))); k++)
				{
					if (k == (n/4))
					{
						s*=-1;
						W++;
					}
					v1 = (n>>2) + j*n + k;
					v2 = Mn*n - 1 - v1;
					
					aux = -y[v1] * (*W);
					aux += y[v2] * (*(W+s)) * s;
					
					W+=c;
								
					y[v2] *= *(W+s);
					y[v2] += y[v1] * (*(W)) * s;
				
					y[v1] = aux;
					
					W-=c;
				}
				if (i != 1) 
					c-=4;

				W++;
				
			}
			
			W += (1<<(i-1));
			
			//Butterflies
			s = 1;
			Mn *= 2;
			for	(rv = 0; rv < Mn; rv++)			//o número de butterflies começa em 1 e dobra a cada laço do for(i=...
			{
				for	(j = 0; j < (n>>2); j++)
				{		
					v1 = j + rv*(n>>1);
					v2 = -j + rv*(n>>1) + ((n>>1)-1);
					aux = y[v1]*s + y[v2];
					y[v2] = -y[v2]*s + y[v1];
					y[v1] = aux;
				}
				s*=-1;
			}
			n>>=1;
		}

		W += (1<<i);  
		c = (2<<i) - 2;
		
		//5 - Multiplicação pelos últimos cossenos e senos
		for	(j = 0; j < (N_2/2); j++)
		{
			v1 = j;
			v2 = N_2-1 - j;
			
			aux =  y[v1] * (*(W+1));
			aux += y[v2] * (*W);
			
			W+=c;
			
			y[v2] *= *(W);
			y[v2] -= y[v1] * (*(W+1));
			
			y[v1] = aux;
			
			W-=c;
			
			c-=4;
			W+=2;
		}
		N>>=1;
		N_2>>=1;
		y=y0;
		W=w0;
		n=N;
	}
}

