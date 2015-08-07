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

bathtub: bathtub.$(SUFFIX_OBJ) resource.$(SUFFIX_OBJ)
	$(KLINK) -obathtub bathtub.$(SUFFIX_OBJ) resource.$(SUFFIX_OBJ) 

clean:
	rm --force *.$(SUFFIX_OBJ) *.ilk *.pdb *.cm *.exe
	rm --force bathtub

bathtub.$(SUFFIX_OBJ): bathtub.c
	$(KC) bathtub.c -o$@

resource.$(SUFFIX_OBJ): resource.c
	$(KC) resource.c -o$@
resource.c: bathtub.svg control.svg
	$(KFILE) resource.c bathtub.svg control.svg
