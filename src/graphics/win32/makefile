#\brief Keystone source file
#
#Keystone Pre Alpha Release
#
#by Jacob Lister - 2002.
#
#contact j_lister@paradise.net.nz
#
#for use by Abbey Systems Wellington internally only
#
#DO NOT DISTRIBUTE

include ../../rules.mak

NATIVE_OBJS  = framework_native.$(SUFFIX_OBJ) gwindow_native.$(SUFFIX_OBJ) gcanvas_native.$(SUFFIX_OBJ) gxulelement_native.$(SUFFIX_OBJ)
ifdef KEYSTONE_WIN32_GDIP
GDIP = -DKEYSTONE_WIN32_GDIP
endif

#>>>dummy rule 'cd .' don't like this
all: $(OBJS) $(NATIVE_OBJS) $(NATIVE_EXTRA)
	cd .

clean:
	$(CLEAN) *.$(SUFFIX_OBJ) *.exe $(EXTRACLEAN)

framework_native.$(SUFFIX_OBJ): framework_native.c $(HEADERS)
	$(KC) $(GDIP) framework_native.c -o$@

gcanvas_native.$(SUFFIX_OBJ): gcanvas_native.c $(HEADERS)
	$(KC) $(GDIP) gcanvas_native.c -o$@

gxulelement_native.$(SUFFIX_OBJ): gxulelement_native.c $(HEADERS)
	$(KC) $(GDIP) gxulelement_native.c -o$@

gwindow_native.$(SUFFIX_OBJ): gwindow_native.c $(HEADERS)
	$(KC) $(GDIP) gwindow_native.c -o$@

gactivexcontainer.$(SUFFIX_OBJ): gactivexcontainer.cpp $(HEADERS)
	$(KC) $(GDIP) gactivexcontainer.cpp -o$@
