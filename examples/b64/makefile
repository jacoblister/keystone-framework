# Keystone Pre Alpha Release
#
# by Jacob Lister - 2002.
#
# contact j_lister@paradise.net.nz
#
# for use by Abbey Systems Wellington internally only
#
# DO NOT DISTRIBUTE

include $(KEYSTONE_DIR)/src/rules.mak

b64: b64.$(SUFFIX_OBJ) cencode.$(SUFFIX_OBJ) cdecode.$(SUFFIX_OBJ)
	keylink -ob64 b64.$(SUFFIX_OBJ) cencode.$(SUFFIX_OBJ) cdecode.$(SUFFIX_OBJ)

clean:
	rm --force *.$(SUFFIX_OBJ) *.ilk *.pdb *.cm *.exe
	rm --force b64

b64.$(SUFFIX_OBJ): b64.c
	keycc b64.c -o$@
	
cencode.$(SUFFIX_OBJ): cencode.c
	$(CC) cencode.c -o$@
	
cdecode.$(SUFFIX_OBJ): cdecode.c
	$(CC) cdecode.c -o$@


