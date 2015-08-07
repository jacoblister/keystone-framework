include $(KEYSTONE_DIR)/src/rules.mak

statetest: statetest.$(SUFFIX_OBJ)
	keylink -ostatetest statetest.$(SUFFIX_OBJ) 

clean:
	rm --force *.$(SUFFIX_OBJ) *.ilk *.pdb *.cm *.exe
	rm --force statetest

statetest.$(SUFFIX_OBJ): statetest.c
	keycc statetest.c -o$@
