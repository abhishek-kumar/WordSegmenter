#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <time.h>
#include <limits.h>


#define DATASET_SIZE 70000 
#define FEATURE_FUN4
#ifdef FEATURE_FUN2
	#define START_POS 1
	#define END_OFFSET 0
	#define FEATURE_SIZE 19600
#endif
#ifdef FEATURE_FUN3 
	#define START_POS 1
	#define END_OFFSET 1
	#define FEATURE_SIZE 548800
#endif
#ifdef FEATURE_FUN4
	#define START_POS 2
	#define END_OFFSET 1
	#define FEATURE_SIZE 15366400
#endif
#ifdef FEATURE_FUN5
	#define START_POS 2
	#define END_OFFSET 1
	#define FEATURE_SIZE 138297600
#endif

enum trainingMethod {ContrastiveDivergence, CollinsPerceptron};
extern int datasetSize;

template <typename T>
void copy(T a[], T b[], int len)
{
	for(int i=0; i<len; ++i)
	{
		b[i] = a[i];
	}
}

/*
float norm(float *m, int len)
{
	float ret = 0;
	for(long i=0; i<len; ++i)
		ret += (m[i])*(m[i]);
  return ret;
}
*/

inline float norm(float *m, float *n, int len)
{
	float ret = 0;
	for(long i=0; i<len; ++i)
		ret += (m[i]-n[i])*(m[i]-n[i]);
  return ret;
}

inline float infiniteNorm(float *m, float *n, int len)
{
	float ret = 0;
	for(long i=0; i<len; ++i)
	{
		float temp = (m[i]-n[i]);
		if(temp > ret && temp > 0)
			ret = temp;
	  else if(temp < (-1)*ret && temp < 0)
		  ret = (-1)*temp;
  }
	return ret;
}

void viterbi(float w[], int x[], int len, int yhat[]);

float CRFTest(int x[][25], int y[][25], int lengths[], float w[]);

float CRFTestLong(int x[][25], int y[][25], int lengths[], float w[]);

float * CRFTrain(int datax[][25], int datay[][25], int lengths[], trainingMethod tm, float *w);

