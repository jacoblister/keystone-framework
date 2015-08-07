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

helloworld: helloworld.$(SUFFIX_OBJ)
	$(KLINK) -ohelloworld helloworld.$(SUFFIX_OBJ) --noconsole

clean:
	rm --force *.$(SUFFIX_OBJ) *.ilk *.pdb *.cm *.exe
	rm --force helloworld

helloworld.$(SUFFIX_OBJ): helloworld.c
	$(KC) helloworld.c -o$@
