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

include ../rules.mak

OBJS       = graphics.$(SUFFIX_OBJ) gobject.$(SUFFIX_OBJ) gpalette.$(SUFFIX_OBJ) gcoord.$(SUFFIX_OBJ) gwindow.$(SUFFIX_OBJ) \
	gprimitive.$(SUFFIX_OBJ) gprimcontainer.$(SUFFIX_OBJ) gproperties.$(SUFFIX_OBJ) gxulelement.$(SUFFIX_OBJ) gcanvas.$(SUFFIX_OBJ) glayout.$(SUFFIX_OBJ) \
	ganimation.$(SUFFIX_OBJ) ganimationinput.$(SUFFIX_OBJ)

ifeq ($(KEYSTONE_TARGET), linux)
NATIVE_BUILD = native_gtk
endif
ifeq ($(KEYSTONE_TARGET), win32)

ifdef KEYSTONE_WIN32_GTK
NATIVE_BUILD = native_win32 native_gtk
else
NATIVE_BUILD = native_win32 
endif

endif

all: $(OBJS) $(NATIVE_EXTRA) $(NATIVE_BUILD)
	cd .

native_win32:
	make -C win32
	
native_gtk:
	make -C gtk

clean:
	$(CLEAN) *.$(SUFFIX_OBJ) *.exe $(EXTRACLEAN)
	make -C win32 clean
	make -C gtk clean
   
.PHONY : doc
doc: $(OBJS:%.$(SUFFIX_OBJ)=%.doc)
	cd .

ganimation.$(SUFFIX_OBJ): ganimation.c
	$(KC) ganimation.c -o$@

ganimationinput.$(SUFFIX_OBJ): ganimationinput.c
	$(KC) ganimationinput.c -o$@

gbitmap.$(SUFFIX_OBJ): gbitmap.c
	$(KC) gbitmap.c -o$@

gcanvas.$(SUFFIX_OBJ): gcanvas.c
	$(KC) gcanvas.c -o$@

gcoord.$(SUFFIX_OBJ): gcoord.c
	$(KC) gcoord.c -o$@

gxulelement.$(SUFFIX_OBJ): gxulelement.c
	$(KC) gxulelement.c -o$@

glayout.$(SUFFIX_OBJ): glayout.c
	$(KC) glayout.c -o$@

gobject.$(SUFFIX_OBJ): gobject.c
	$(KC) gobject.c -o$@

gpalette.$(SUFFIX_OBJ): gpalette.c
	$(KC) gpalette.c -o$@

gprimitive.$(SUFFIX_OBJ): gprimitive.c
	$(KC) gprimitive.c -o$@

gprimcontainer.$(SUFFIX_OBJ): gprimcontainer.c
	$(KC) gprimcontainer.c -o$@

gproperties.$(SUFFIX_OBJ): gproperties.c
	$(KC) gproperties.c -o$@

graphics.$(SUFFIX_OBJ): graphics.c
	$(KC) graphics.c -o$@

gwindow.$(SUFFIX_OBJ): gwindow.c
	$(KC) gwindow.c -o$@