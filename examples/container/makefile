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

helloworld: string.$(SUFFIX_OBJ)
	keylink -ostring string.$(SUFFIX_OBJ) 

clean:
	rm --force *.$(SUFFIX_OBJ) *.ilk *.pdb *.cm *.exe
	rm --force string

string.$(SUFFIX_OBJ): string.c
	keycc string.c -o$@
