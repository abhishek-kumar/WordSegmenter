ABOUT
WordSegmenter is a word segmentation system that identifies sequences of syllables in English words. These words are represented using the BIO method of coding syllables, which relies on tagging each letter of a word based on its position in the syllable. 

We use a linear chain conditional random field model, which uses a large number indicator functions that rely on (1) the letter sequences in a word, (2) the position of these letters in the word, and (3) the encoded tag sequence of syllables for the letter sequence. 

This program evaluates two different methods to train the parameters of this model(Contrastive Divergence and Collin's Perceptron) using a subset of the CELEX European lexical resource data set. 

While evaluation, the best word level accuracy achieved using this model, on a separate test set is 85.69%. When evaluated on the training dataset, the word level accuracy  achieved is 94.3%.


RUNNING
The program takes 3 inputs or arguments:
  1. The training set
	2. The test set to evaluate the model
	3. The choice of method used to train the model

In order to compile the source, run:
$> g++ CRFWordSegmenter.cpp -o WordSegmenter

In order to run the code, 
$> ./ WordSegmenter train_set_60k.txt test_set_6k.txt [c|d]
where c stands for collin's perceptron method, while d stands for contrastive divergence.
