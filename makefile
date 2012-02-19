all:
	cd src; $(MAKE) all
	@echo Finished compiling

clean:
	cd src; $(MAKE) clean
	@echo Finished cleanup
