TABLE OF CONTENTS

1. About
2. Quickstart
3. Introduction
4. Compiling
5. Training
6. Testing
7. Additional remarks


1. ABOUT
   WordSegmenter is a word segmentation system that identifies
   sequences of syllables in English words. A common application
   of this is in hyphenation of english words.

2. QUICKSTART
   Download this project, and navigate to the root directory
   through the command-line (the directory that contains this
   README.txt file).

   Then type:
     $> make all
     $> ./bin/WordSegmenter ./data/w.data

   Now, start typing english words (without punctuation marks 
   or spaces) and press enter after each word. The hyphenated
   word will be displayed on the next line.

3. INTRODUCTION
   This program addresses the task of automatic syllabification
   of words. These words are represented using the BIO method of
   coding syllables, which relies on tagging each letter of a 
   word based on its position in the syllable. 

   We use a linear chain conditional random field model, which
   uses a large number indicator functions that rely on (1) the
   letter sequences in a word, (2) the position of these letters 
   in the word, and (3) the encoded tag sequence of syllables for
   the letter sequence. 

   This program evaluates two different methods to train the 
   parameters of this model(Contrastive Divergence and 
   Collin's Perceptron) using a subset of the CELEX European lexical 
   resource data set. 

   While evaluation, the best word level accuracy achieved using 
   this model, on a separate test set is 85.69%. When evaluated 
   on the training dataset, the word level accuracy  achieved 
   is 94.3%.

   Further details are provided in the paper: 
   http://dl.dropbox.com/u/3091691/Papers/WordSyllabificationUsingCRF.pdf

4. COMPILING
   The program can be compiled using the Unix make utility (or gmake)
   'make all' will create all the binary files in the 'bin' directory.
   Note that the program is sensitive to the name of the binary, and
   so they must always be named 'WordSegmenter', 'Train' and 'Test'

5. TRAINING
   The CRF model can be trained by providing a training data file
   and specifying which method to use for training.
   Command:
     $> ./bin/Train <training data file> [c|d] <output file>
   training data file: this is a file that contains words and their
                       correct output labels. A sample training
                       data file is provided in the 'data' 
                       directory, called 'train_set_60k.data'. This
                       file was obtained after pre-processing the
                       CELEX dataset, as explained in the paper
                       whose link you can find at the end of this file.
   [c|d]: This specifies the training method to use. 'c' indicates
                       Collin's Perceptron, while 'd' indicates 
                       Contrastive Divergence. Both of these are 
                       methods to approximate the stochastic gradient
                       descent update to the model parameters as 
                       explained in the paper.
   output file: This specifies the file where the parameter values
                       will be written. This file is needed to perform
                       testing, and to run 'WordSegmenter'
6. TESTING
   After training, the model can be tested by
     $> ./bin/Test <test set file> <parameter file>
   test set file: A sample test set file is available in the 'data'
                       directory: 'test_set_6k.data'
   parameter file: This should be the file obtained after training.
                       The file 'data/w.data' is the file we provide
                       after our training.
7. ADDITIONAL COMMENTS
   The hyphenation task has a high accuracy, but in terms of recall. 
   The precision still needs improvement. So, when you run the WordSegmenter, 
   you will find that it places hyphens correctly at locations which
   need one, but it also place a few stray additional hyphens in some
   cases (false positives). In general, this tends to occur after
   the first letter.

   A further description of the tool can be found here:
   http://dl.dropbox.com/u/3091691/Papers/WordSyllabificationUsingCRF.pdf

   In order to hyphenate many words, you would need to pass these
   words to the standard input of the program, separated by a newline.
   Another way to pass words is to use the second argument
     $> ./bin/WordSegmenter ./data/w.data yourwordgoeshere
