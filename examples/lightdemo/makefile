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

all: lightdemo1 lightdemo2 lightdemo3 lightdemo4

lightdemo1: lightdemo1.$(SUFFIX_OBJ) 
	$(KLINK) -olightdemo1 lightdemo1.$(SUFFIX_OBJ) 

lightdemo2: lightdemo2.$(SUFFIX_OBJ) 
	$(KLINK) -olightdemo2 lightdemo2.$(SUFFIX_OBJ) 
	
lightdemo3: lightdemo3.$(SUFFIX_OBJ) 
	$(KLINK) -olightdemo3 lightdemo3.$(SUFFIX_OBJ) 
	
lightdemo4: lightdemo4.$(SUFFIX_OBJ) 
	$(KLINK) -olightdemo4 lightdemo4.$(SUFFIX_OBJ) 

clean:
	rm --force *.$(SUFFIX_OBJ) *.ilk *.pdb *.cm *.exe
	rm --force lightdemo1 lightdemo2

lightdemo1.$(SUFFIX_OBJ): lightdemo1.c
	$(KC) lightdemo1.c -o$@

lightdemo2.$(SUFFIX_OBJ): lightdemo2.c
	$(KC) lightdemo2.c -o$@
	
lightdemo3.$(SUFFIX_OBJ): lightdemo3.c 
	$(KC) lightdemo3.c -o$@	
	
lightdemo4.$(SUFFIX_OBJ): lightdemo4.c 
	$(KC) lightdemo4.c -o$@	

