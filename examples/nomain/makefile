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

nomain: nomain.$(SUFFIX_OBJ)
	$(KLINK) -onomain nomain.$(SUFFIX_OBJ)

clean:
	rm --force *.$(SUFFIX_OBJ) *.ilk *.pdb *.cm *.exe
	rm --force nomain

nomain.$(SUFFIX_OBJ): nomain.c
	$(KC) nomain.c -o$@
