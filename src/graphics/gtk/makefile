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
ifdef KEYSTONE_GTK_CAIRO
CAIRO = -DKEYSTONE_GTK_CAIRO
endif

ifeq ($(KEYSTONE_TARGET), win32)
#>>>dummy rule 'cd .' don't like this
all: gtk.rsp $(OBJS) $(NATIVE_OBJS) $(NATIVE_EXTRA)
	rm gtk.rsp

clean:
	$(CLEAN) *.$(SUFFIX_OBJ) *.exe gtk.rsp $(EXTRACLEAN) 

gtk.rsp:
	pkg-config --cflags gtk+-2.0 --msvc-syntax >gtk.rsp

framework_native.$(SUFFIX_OBJ): framework_native.c $(HEADERS)
	$(KC) @gtk.rsp $(CAIRO) framework_native.c -o$@

gcanvas_native.$(SUFFIX_OBJ): gcanvas_native.c $(HEADERS)
	$(KC) @gtk.rsp $(CAIRO)  gcanvas_native.c -o$@

gxulelement_native.$(SUFFIX_OBJ): gxulelement_native.c $(HEADERS)
	$(KC) @gtk.rsp $(CAIRO) gxulelement_native.c -o$@

gwindow_native.$(SUFFIX_OBJ): gwindow_native.c $(HEADERS)
	$(KC) @gtk.rsp $(CAIRO) gwindow_native.c -o$@
endif

ifeq ($(KEYSTONE_TARGET), linux)
#>>>dummy rule 'cd .' don't like this
all: $(OBJS) $(NATIVE_OBJS) $(NATIVE_EXTRA)
	cd .

clean:
	$(CLEAN) *.$(SUFFIX_OBJ) *.exe gtk.rsp $(EXTRACLEAN) 

framework_native.$(SUFFIX_OBJ): framework_native.c $(HEADERS)
	$(KC) `pkg-config --cflags gtk+-2.0` $(CAIRO) framework_native.c -o$@

gcanvas_native.$(SUFFIX_OBJ): gcanvas_native.c $(HEADERS)
	$(KC) `pkg-config --cflags gtk+-2.0` $(CAIRO) gcanvas_native.c -o$@

gxulelement_native.$(SUFFIX_OBJ): gxulelement_native.c $(HEADERS)
	$(KC) `pkg-config --cflags gtk+-2.0` $(CAIRO) gxulelement_native.c -o$@

gwindow_native.$(SUFFIX_OBJ): gwindow_native.c $(HEADERS)
	$(KC) `pkg-config --cflags gtk+-2.0` $(CAIRO) gwindow_native.c -o$@
endif