include $(KEYSTONE_DIR)/src/rules.mak

string: string.$(SUFFIX_OBJ)
	keylink -ostring string.$(SUFFIX_OBJ) 

clean:
	rm --force *.$(SUFFIX_OBJ) *.ilk *.pdb *.cm *.exe
	rm --force string

string.$(SUFFIX_OBJ): string.c
	keycc string.c -o$@
