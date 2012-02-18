#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <time.h>
#include <limits.h>
using namespace std;

	typedef int dimensions[25];
//#define DATASET_SIZE 100
#define DATASET_SIZE 67010
int datasetSize = 0;
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
void viterbi(float w[], int x[], int len, int yhat[]);

int is_vow(int c)
{
	switch (c) {
		case 0:
			return 1;
		case 4:
			return 1;
		case 8:
			return 1;
		case 14:
			return 1;
		case 20:
			return 1;
	}
	return 0;
}

template <typename T>
void copy(T a[], T b[], int len)
{
	for(int i=0; i<len; ++i)
	{
		b[i] = a[i];
	}
}

float norm(float *m, int len)
{
	float ret = 0;
	for(long i=0; i<len; ++i)
		ret += (m[i])*(m[i]);
  return ret;
}

float norm(float *m, float *n, int len)
{
	float ret = 0;
	for(long i=0; i<len; ++i)
		ret += (m[i]-n[i])*(m[i]-n[i]);
  return ret;
}

float infiniteNorm(float *m, float *n, int len)
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

float CRFTest(int x[][25], int y[][25], int lengths[], float w[])
{
	int y_hat[25];
	int len;
	bool accurate = false;
	int *y_current;
	int count_correct = 0;
	int count_total = 0;
	//FILE * rFile = fopen("results.txt", "w");
	for(int i = 0; i<datasetSize; ++i)
	{
		len = lengths[i];
		++count_total;
		y_current = y[i];

		//Estimate y_hat
		viterbi(w, x[i], len, y_hat);	
		
		//Compare Estimate to y
		accurate = true;
		for(int j=0; j<len; ++j)
			if(y_hat[j] != y_current[j])
				accurate = false;
		if(accurate)
			++count_correct;
			/*
		for(int j=0; j<len; ++j)
			fprintf(rFile, "%d", y_hat[j]);
		fprintf(rFile, ";");
		for(int j=0; j<len; ++j)
			fprintf(rFile, "%d", y_current[j]);
		fprintf(rFile, "\n");
		*/
	}
	//fclose(rFile);
	return (float(count_correct)/float(count_total));
}

float CRFTestLong(int x[][25], int y[][25], int lengths[], float w[])
{
	int y_hat[25];
	int len;
	bool accurate = false;
	int *y_current;
	int count_correct = 0;
	int count_total = 0;
	for(int i = 0; i<datasetSize; ++i)
	{
		len = lengths[i];
		if(len < 5)
			continue;
		++count_total;
		y_current = y[i];

		//Estimate y_hat
		viterbi(w, x[i], len, y_hat);	
		
		//Compare Estimate to y
		accurate = true;
		for(int j=0; j<len; ++j)
			if(y_hat[j] != y_current[j])
				accurate = false;
		if(accurate)
			++count_correct;
			/*
		for(int j=0; j<len; ++j)
			fprintf(rFile, "%d", y_hat[j]);
		fprintf(rFile, ";");
		for(int j=0; j<len; ++j)
			fprintf(rFile, "%d", y_current[j]);
		fprintf(rFile, "\n");
		*/
	}
	return (float(count_correct)/float(count_total));
}
void computeG(float w[], int x[], int m, int len, float  G[][5][5])
{
	float temp = 0.0;

	for(int i=START_POS; i<len-END_OFFSET; ++i)
	{
		for(int tag1=0; tag1<m; ++tag1)
			for(int tag2=0; tag2<m; ++tag2)
			{
				#ifdef FEATURE_FUN2
					long feat = x[i-1]*700 + x[i]*25 + tag1*5 + tag2;
					G[i][tag1][tag2] = w[feat];
				#endif
				#ifdef FEATURE_FUN3
					int feat = x[i-1]*19600 + x[i]*700 + x[i+1]*25 + tag1*5 + tag2;
   	     G[i][tag1][tag2] = w[feat];
				#endif
				#ifdef FEATURE_FUN4
					int feat = x[i-2]*19600*28+x[i-1]*19600 + x[i]*700 + x[i+1]*25 + tag1*5 + tag2;
   	     G[i][tag1][tag2] = w[feat];
				#endif
				#ifdef FEATURE_FUN5
					int vow1,vow2;
					if (i<3) {
						vow1 = 1;
					}
					else {
						vow1 = 2*is_vow(x[i-3]);
					}
					if (i>len-3) {
						vow2 = 1;
					}
					else {
						vow2 = 2*is_vow(x[i+2]);
					}
					int feat = vow1*46099200+vow2*15366400+x[i-2]*548800+x[i-1]*19600 + x[i]*700 + x[i+1]*25 + tag1*5 + tag2;
   	      G[i][tag1][tag2] = w[feat];
				#endif
			}
	}
}

void viterbi(float w[], int x[], int len, int yhat[]) 
{
	//Given w,x, we want to compute yhat
  int m=5;
	float G[24][5][5];
	int alpha[24][5];
  int DEC[24][5];

	computeG(w, x, m, len, G);

	//initialize alpha[0][v]
	for (int v=0;v<m;v++) {
		alpha[0][v] = INT_MIN;
	}
	alpha[0][3] = 0;

	//compute alpha[0..len-1][v] and DEC[0..len-1][v]
  for (int i=1;i<len;i++) {
		for (int v=0;v<m;v++) {
			float max_alpha = INT_MIN;
      DEC[i][v] = 0;
			for (int u=0;u<m;u++) {
 				float alpha_value = alpha[i-1][u] + G[i][u][v];
				if (alpha_value > max_alpha) {
					alpha[i][v] = alpha_value;
          max_alpha = alpha_value;
          DEC[i][v] = u;
				}
			}
		}
	}
	yhat[len-1] = 4;
	for (int i=len-2;i>=0;i--) {
		yhat[i] = DEC[i+1][yhat[i+1]];
	}
}

void F_add(int x[], int y[], int len, float w[],float diff)
{
	for(int i=START_POS; i<len-END_OFFSET; ++i)
	{
		#ifdef FEATURE_FUN2
			int feat = x[i-1]*700 + x[i]*25 + y[i-1]*5 + y[i];
			w[feat] += diff;
		#endif
		#ifdef FEATURE_FUN3
			int feat = x[i-1]*19600 + x[i]*700 + x[i+1]*25 + y[i-1]*5 + y[i];
			w[feat] += diff;
		#endif
		#ifdef FEATURE_FUN4
			int feat = x[i-2]*19600*28+x[i-1]*19600 + x[i]*700 + x[i+1]*25 + y[i-1]*5 + y[i];
			w[feat] += diff;
		#endif
		#ifdef FEATURE_FUN5
			int vow1,vow2;
			if (i<3) {
				vow1 = 1;
			}
			else {
				vow1 = 2*is_vow(x[i-3]);
			}
			if (i>len-3) {
				vow2 = 1;
			}
			else {
				vow2 = 2*is_vow(x[i+2]);
			}
			int feat = vow1*46099200+vow2*15366400+x[i-2]*548800+x[i-1]*19600 + x[i]*700 + x[i+1]*25 + y[i-1]*5 + y[i];
			w[feat] += diff;
		#endif
	}
}

void sample(float G[][5][5], int position, int y[])
{
	float p0, p1, p2, p3, p4, denominator;
	float p[5];

	denominator=0;
  int m=3;
	for(int tag=0; tag<m; ++tag)
	{
		p[tag] = exp(G[position][y[position-1]][tag] + G[position+1][tag][y[position+1]]);
		denominator += p[tag];
	}

	for(int tag=0; tag<m; ++tag)
	{
		p[tag] = p[tag] / denominator;
	}

	float r = rand() / ((float) RAND_MAX); r*=(p[0]+p[1]+p[2]);
	if(r<p[0])
		y[position] = 0;
	else if(r<(p[0]+p[1]))
		y[position] = 1;
	else if(r<(p[0]+p[1]+p[2]))
	  y[position] = 2;
		/*
  else if(r<(p[0]+p[1]+p[2]+p[3]))
	  y[position] = 3;
	else
		y[position] = 4;
		*/
}

void readFile(int x[][25], int y[][25], int lengths[], char *filename)
{
	std::ifstream input (filename);
	std::string lineData;
	int linenumber=-1; int xlength, ylength;
	while(getline(input, lineData))
	{
		int d; int i=0;
		++linenumber;
		xlength = 0; ylength = 0;
		std::stringstream lineStream(lineData);
		x[linenumber][0] = 26;
		y[linenumber][0] = 3;
		while (lineStream >> d)
		{
			++i;
			if(i<23) //Reading x
			{
				x[linenumber][i] = d;
//				printf(" %d", d);
				if(d != -1)
					++xlength;
			}
			else //reading y
			{
				y[linenumber][i-22] = d;
//				printf("_%d", d);
				if(d != -1)
					++ylength;
			}
		}
		xlength+=2; ylength+=2;
		x[linenumber][xlength-1] = 27;
		y[linenumber][ylength-1] = 4;
		if(xlength != ylength)
			throw "x and y lengths do not match";
		lengths[linenumber] = xlength;
//		printf("\n%d\n", xlength);
	}
	datasetSize = linenumber + 1;
}

float * CRFTrain(int datax[][25], int datay[][25], int lengths[], trainingMethod tm, float *w)
{
	/*
	 * This function learns weights w from the training set provided, and returns this (w)
	 * Make sure w is all zeros at the beginning
	 */
  float *w_old = (float*)malloc(sizeof(float)*FEATURE_SIZE);
	srand(time(0));
  float convergence[1000];

	int y_hat[30];
	int m=5, n=0; //tags and word size

	float G[24][5][5];
	float lambda_cd = 0.01;
	float lambda_cp = 10; 
	float lambda_decay = 0.83;
  float lambda; //to be set to either cp or cd's lambda
  int *x; int *y;
  FILE * wFile = fopen("wla.txt", "w");

	for(int epoch=0; epoch<150; ++epoch)
	{
		copy(w,w_old,FEATURE_SIZE);
		for(int example=0; example<datasetSize; ++example)
		{
			x = datax[example];
			y = datay[example];
			n = lengths[example];
			if(tm == ContrastiveDivergence)
			{
				//compute y_hat
				copy(y, y_hat, n);
				computeG(w, x, m, n, G);
				for(int i=1; i<(n-1); ++i)
				{
					sample(G, i, y_hat);
				}
				lambda = lambda_cd;
			}
			else
			{
			  //compute y_hat
				viterbi(w, x, n, y_hat);
				lambda = lambda_cp;
			}
			
      F_add(x, y, n, w,lambda);
			F_add(x, y_hat, n, w,(-1)*lambda);
			lambda = lambda*lambda_decay;
		}
		float wdiff = norm(w,w_old,FEATURE_SIZE);
		//printf("EPOCH %d, change in w = %10.5f ", epoch, wdiff);
		//printf(" ");
		//Calculate intermediate accuracy
		float a = CRFTest(datax, datay, lengths, w);
		fprintf(wFile, "%f\n", a);
		printf(" Word Level Accuracy = %f\n", a);
	}
	free(w_old);
	fclose(wFile);
}

int main(int argc, char** argv)
{
	if(argc<4)
	{
		cout << "Please provide filenames for the training / test data " 
		     << "and the training method\n"
				 << "Format: $> binary trainfile testfile [c|d]\nQuitting.\n" << std::endl;
		return 0;
	}
  
	dimensions *x = new dimensions[DATASET_SIZE];
	dimensions *y = new dimensions[DATASET_SIZE];
	int *lengths = new int[DATASET_SIZE];
  float *w = (float*)malloc(sizeof(float)*FEATURE_SIZE);
	for(int i=0; i<FEATURE_SIZE; ++i)
		w[i] = 0;


	readFile(x, y, lengths, argv[1]);
	
	//TRAIN
	if(argv[2][0] == 'd')
		CRFTrain(x, y, lengths, ContrastiveDivergence, w);
	else
		CRFTrain(x, y, lengths, CollinsPerceptron, w);
  int fc = 0;

	//Test
	readFile(x, y, lengths, argv[2]);
	float accuracy = CRFTestLong(x, y, lengths, w);
	printf("Overall Accuracy: %f\n", accuracy);
	free(w);
}

