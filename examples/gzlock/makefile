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

all: gzlock gzunlock

gzlock: gzlock.$(SUFFIX_OBJ)
	keylink -ogzlock gzlock.$(SUFFIX_OBJ) 
	
gzunlock: gzunlock.$(SUFFIX_OBJ)
	keylink -ogzunlock gzunlock.$(SUFFIX_OBJ) 

clean:
	rm --force *.$(SUFFIX_OBJ) *.ilk *.pdb *.cm *.exe
	rm --force gzlock gzunlock

gzlock.$(SUFFIX_OBJ): gzlock.c
	keycc gzlock.c -o$@
	
gzunlock.$(SUFFIX_OBJ): gzunlock.c
	keycc gzunlock.c -o$@
