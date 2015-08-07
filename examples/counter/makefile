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

RESOURCE_OBJS = counter.svg

counter: counter.$(SUFFIX_OBJ) resource.$(SUFFIX_OBJ)
	$(KLINK) -ocounter counter.$(SUFFIX_OBJ) resource.$(SUFFIX_OBJ) --noconsole

clean:
	rm --force *.$(SUFFIX_OBJ) *.ilk *.pdb *.cm *.exe
	rm --force counter

counter.$(SUFFIX_OBJ): counter.c
	$(KC) counter.c -o$@

resource.$(SUFFIX_OBJ): resource.c
	$(KC) resource.c -o$@
resource.c: $(RESOURCE_OBJS)
	$(KFILE) resource.c $(RESOURCE_OBJS)
