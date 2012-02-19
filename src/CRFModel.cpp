#include "WordSegmenter.h"
using namespace std;

int datasetSize = 0;

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

float CRFTest(int x[][25], int y[][25], int lengths[], float w[])
{
	/**
	 *  This function tests a model (w) on a test set (x,y)
	 *  The metric is Word-Level-Accuracy
	 */
	int y_hat[25];
	int len;
	bool accurate = false;
	int *y_current;
	int count_correct = 0;
	int count_total = 0;
	
	for(int i = 0; i<datasetSize; ++i)
	{
		len = lengths[i];
		++count_total;
		y_current = y[i];

		//Estimate y_hat
		viterbi(w, x[i], len, y_hat);	
		
		//Compare Estimate to y
		accurate = true;
		for(int j=1; j<(len-1); ++j)
			if(y_hat[j] != y_current[j])
				accurate = false;
		if(accurate)
			++count_correct;
	}
	return (float(count_correct)/float(count_total));
}

float CRFTestLong(int x[][25], int y[][25], int lengths[], float w[])
{
	/**
	 *  This function tests a model (w) on a test set (x,y)
	 *  ONLY for long words (greater than 3 letters) since
	 *  Word segmentation makes sense for these words only
	 *  The metric is Word-Level-Accuracy
	 */
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
		for(int j=1; j<(len-1); ++j)
			if(y_hat[j] != y_current[j])
				accurate = false;
		if(accurate)
			++count_correct;
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
	yhat[0] = 3;
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
}


float * CRFTrain(int datax[][25], int datay[][25], int lengths[], trainingMethod tm, float *w)
{
	/*
	 * This function learns weights w from the training set provided, 
	 * and returns this (w)
	 * Make sure w is all zeros at the beginning
	 */
  float *w_old = (float*)malloc(sizeof(float)*FEATURE_SIZE);
	srand(time(0));
  float convergence[1000];

	int y_hat[30];
	int m=5, n=0; //tags and word size

	float G[24][5][5];
	float lambda_cd = 0.001;
	float lambda_cp = 0.1; 
	float lambda_decay = 1;
  float lambda; //to be set to either cp or cd's lambda
  int *x; int *y;

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
		//Change in parameters w; second-norm
		float wdiff = norm(w,w_old,FEATURE_SIZE);

		//Calculate accuracy on train set with current model
		float a = CRFTest(datax, datay, lengths, w);

		printf("\tEpoch #%d,\tChange in model parameters: %2.5f\t", epoch, wdiff);
		//printf("\tEpoch #%d\n", epoch);
		printf(" Word Level Accuracy:  %f\n", a);
	}
	free(w_old);
}

