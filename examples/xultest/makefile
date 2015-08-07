# Keystone Pre Alpha Release
#
# by Jacob Lister - 2002.
#
# contact j_lister@paradise.net.nz
#
# for use by Abbey Systems Wellington internally only
#
# DO NOT DISTRIBUTE

include ../../src/rules.mak

RESOURCEOBJ  = colour.svg

xultest: xultest.$(SUFFIX_OBJ) resource.$(SUFFIX_OBJ)
	$(KLINK) -oxultest xultest.$(SUFFIX_OBJ) resource.$(SUFFIX_OBJ) 

clean:
	rm --force *.$(SUFFIX_OBJ) *.ilk *.pdb *.cm *.exe
	rm --force xultest

xultest.$(SUFFIX_OBJ): xultest.c
	$(KC) xultest.c -o$@
	
resource.$(SUFFIX_OBJ): resource.c
	$(KC) resource.c -o$@
resource.c: $(RESOURCEOBJ)
	$(KLINK) resource.c $(RESOURCEOBJ)