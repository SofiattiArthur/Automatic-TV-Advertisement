#ifndef __MFCC_H
#define	__MFCC_H

#include "DCT.h"
#include "FFT.h"
#include <stdlib.h>
#include <math.h>

void hann(float *x, unsigned int L);

int mfcc(float** fo, unsigned int nfft, unsigned int step, unsigned int ncoef, float *x, unsigned int fs, unsigned int s);

#endif