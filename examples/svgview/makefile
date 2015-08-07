include ../../src/rules.mak

svgview: svgview.$(SUFFIX_OBJ)
	$(KLINK) -osvgview svgview.$(SUFFIX_OBJ) 

clean:
	rm --force *.$(SUFFIX_OBJ) *.ilk *.pdb *.cm *.exe
	rm --force svgview

svgview.$(SUFFIX_OBJ): svgview.c
	$(KC) svgview.c -o$@ 