CC=g++
CCFLAGS=
BINPATH=../bin

SRCS=\
CRFModel.cpp\
WordSegmenter.cpp

HDRS=\
WordSegmenter.h

WordSegmenter:	$(SRCS)
	$(CC) $(CCFLAGS) $(SRCS) -o $(BINPATH)/WordSegmenter

Train:	$(SRCS)
	$(CC) $(CCFLAGS) $(SRCS) -o $(BINPATH)/Train

Test:	$(SRCS)
	$(CC) $(CCFLAGS) $(SRCS) -o $(BINPATH)/Test

all:
	touch $(SRCS)
	make WordSegmenter
	make Train
	make Test
	@echo Finished compiling all files.

clean:
	rm -rf $(BINPATH)/*
	@echo Directory $(BINPATH) cleaned up.

