#ifndef __FFT_H
#define __FFT_H

#include <stdlib.h>
#include <math.h>

float* coef_wk(unsigned int n);

int btrv(int i,int l);

void bitrev(int n,float *ptrt,float *ptrf);

int fft(unsigned int n,float *ptrt,float *ptrf,float *ptrw);
	
#endif
