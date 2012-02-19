#include "WordSegmenter.h"
using namespace std;

extern int datasetSize;
typedef int dimensions[25];

#ifndef READFILE
#define READFILE
void readFile(int x[][25], int y[][25], int lengths[], const char *filename)
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
#endif
bool segmentWord(const char *rawWord, float *w, char *output)
{
	int input[30]; int yhat[30];
	input[0] = 26; yhat[0] = 3;
	const char *c = rawWord;
  char *o;

	int i=0;
	while(*c != '\0' && *c != '\n')
	{
		++i;
		input[i] = *c - 'a';
		if(input[i] < 0 || input[i] > 25)
			return false;
		++c;
	}
	++i;
	input[i] = 27; yhat[i] = 4;

	//Calculate output sequence
	viterbi(w, input, i+1, yhat);

	//format output word
	o = output;
	*o = *rawWord;
	++o;
	for(int j=2; j<i; ++j)
	{
		if(yhat[j] == 0)
		{
			*o = '-';
			++o;
		}
		*o = 'a' + input[j];
		++o;
	}
	*o = '\0';
	return true;
}

void readParameters(const char * parameterFile, float *w)
{
	std::ifstream input(parameterFile);
	std::string lineData;
	int i; float d;
	while(getline(input, lineData))
	{
		std::stringstream lineStream(lineData);
		lineStream >> i;
		lineStream >> d;
		w[i] = d;
	}

}

void test(const char *datafile, const char * modelfile, float& a, float& along)
{

	dimensions *x = new dimensions[DATASET_SIZE];
	dimensions *y = new dimensions[DATASET_SIZE];
	int *lengths = new int[DATASET_SIZE];
 	float *w = (float*)malloc(sizeof(float)*FEATURE_SIZE);
		
	for(int i=0; i<FEATURE_SIZE; ++i)
		w[i] = 0;

	readParameters(modelfile, w);
	readFile(x, y, lengths, datafile);

	along = CRFTestLong(x, y, lengths, w);
  a = CRFTest(x, y, lengths, w);
	free(w);
}

void train(const char * datafile, const char * modelfile, trainingMethod tm)
{
	/*
	 * Train a model from data in datafile
	 * and store the model in modelfile
	 */

	dimensions *x = new dimensions[DATASET_SIZE];
	dimensions *y = new dimensions[DATASET_SIZE];
	int *lengths = new int[DATASET_SIZE];
  float *w = (float*)malloc(sizeof(float)*FEATURE_SIZE);

	for(int i=0; i<FEATURE_SIZE; ++i)
		w[i] = 0;


	readFile(x, y, lengths, datafile);
	
	//TRAIN
	CRFTrain(x, y, lengths, tm, w);
	
	//Write to file
  FILE * mFile = fopen(modelfile, "w");
	for(int i=0; i<FEATURE_SIZE; ++i)
	{
		if(w[i] != 0.0)
		{
			fprintf(mFile, "%d\t%4.8f\n", i, w[i]);
		}
	}
	fclose(mFile);
}

int main(int argc, char** argv)
{
	int filenameLength = 0;
	filenameLength = strlen(argv[0]);

  if(strcmp(argv[0]+(filenameLength - 5),"Train")==0)
	  if(argc<4)
	  {
		  cerr << "Please provide the filename for the training data " 
		       << ", the training method, and the output filename\n"
			  	 << "Format: $> ./Train <trainfile> [c|d] <outputfile>" << endl
					 << "Quitting." << std::endl;
		  return 0;
	  }
		else
		{
			train(argv[1], argv[3], 
					 (argv[2][0]=='c'?CollinsPerceptron:ContrastiveDivergence));
			return 0;
		}
  else if(strcmp(argv[0]+(filenameLength - 13),"WordSegmenter")==0)
	{
	  if(argc<2)
	  {
		  cerr << "Please provide the filename for the Parameter File" 
			  	 << "Format: $> ./WordSegmenter <parameterfile>" << endl
					 << "Quitting." << std::endl;
		  return 0;
	  }
		char segmentedWord[50];
  	float *w = (float*)malloc(sizeof(float)*FEATURE_SIZE);
		readParameters(argv[1], w);

		if(argc > 2)
		{
			if(!segmentWord(argv[2], w, segmentedWord))
				cerr << "Badly formatted word: " << argv[1] << std::endl;
			cout << segmentedWord << std::endl;
			return 0;
	  }
		else
		{
			string rawWord;
			while(cin >> rawWord)
			{
				if(!segmentWord(rawWord.c_str(), w, segmentedWord))
					cerr << "Badly formatted word: " << rawWord << std::endl;
				cout << segmentedWord << std::endl;
			}
		}
		free(w);
	}
	else if(strcmp(argv[0]+(filenameLength - 4),"Test")==0)
	{
		if(argc < 3)
		{
		  cerr << "Please provide filename for the test data, and the trained parameter file" << std::endl 
			  	 << "Format: $> ./Test <testfile> <parameterfile>" << endl
					 << "Quitting." << std::endl;
			return 0;
		}
		//Test
		float a, along;
		test(argv[1], argv[2], a, along);
		printf("Accuracy (all words): %f\nAccuracy (words longer than 3 letters): %f\n", a, along);
	}

}

