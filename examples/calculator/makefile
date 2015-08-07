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

RESOURCE_OBJS = skin_svg.svg skin_svg_curves.svg skin_xul.svg skin_hybrid.svg

calculator: calculator.$(SUFFIX_OBJ) resource.$(SUFFIX_OBJ)
	$(KLINK) -ocalculator calculator.$(SUFFIX_OBJ) resource.$(SUFFIX_OBJ) --noconsole 

clean:
	rm --force *.$(SUFFIX_OBJ) *.ilk *.pdb *.cm *.exe
	rm --force calculator

calculator.$(SUFFIX_OBJ): calculator.c
	$(KC) calculator.c -o$@ 

resource.$(SUFFIX_OBJ): resource.c
	$(KC) resource.c -o$@
resource.c: $(RESOURCE_OBJS)
	$(KFILE) resource.c $(RESOURCE_OBJS)
