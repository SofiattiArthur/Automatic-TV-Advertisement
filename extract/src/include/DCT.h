#ifndef __DCT_H
#define	__DCT_H

#include "FFT.h"
#include <math.h>

void bitrev2(int n,float *ptrt,float *ptrf);

float* W_coef(unsigned int N);

int litDCT(unsigned int N, float *x, float *ans);

int dct(unsigned int N, float *x, float *y, float *W);
	
#endif
