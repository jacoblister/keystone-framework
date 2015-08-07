# Keystone Pre Alpha Release
#
# by Jacob Lister - 2002.
#
# contact j_lister@paradise.net.nz
#
# for use by Abbey Systems Wellington internally only
#
# DO NOT DISTRIBUTE

include rules.mak

INC     = $(FRAMEINC)

HEADERS = $(INC)/*.h

OBJS_BASE    = framework_base.$(SUFFIX_OBJ) main.$(SUFFIX_OBJ) object.$(SUFFIX_OBJ) datetime.$(SUFFIX_OBJ) \
syncobject.$(SUFFIX_OBJ) socket.$(SUFFIX_OBJ) event.$(SUFFIX_OBJ) cstring.$(SUFFIX_OBJ) memfile.$(SUFFIX_OBJ) \
objdataserver.$(SUFFIX_OBJ) objpersistent.$(SUFFIX_OBJ) objcontainer.$(SUFFIX_OBJ) 
OBJS_FILE    = file.$(SUFFIX_OBJ) objpersistent_xml.$(SUFFIX_OBJ)
OBJS_KC      = base/contain.$(SUFFIX_OBJ) base/exception.$(SUFFIX_OBJ) base/text.$(SUFFIX_OBJ) cencode.$(SUFFIX_OBJ) cdecode.$(SUFFIX_OBJ)
OBJS_NATIVE_MAIN = main_native.$(SUFFIX_OBJ) thread.$(SUFFIX_OBJ)
OBJS_NATIVE_NOMAIN = main_native_NOMAIN.$(SUFFIX_OBJ) thread.$(SUFFIX_OBJ)

ifeq ($(KEYSTONE_TARGET), linux)
NATIVE_DIR   = linux
OBJS    = $(OBJS_FILE) $(OBJS_BASE) $(OBJS_NATIVE)
GRLIBS  = graphics/gtk/*.$(SUFFIX_OBJ)
LIBS    = graphics/*.$(SUFFIX_OBJ) expat/*.$(SUFFIX_OBJ) extras/*.$(SUFFIX_OBJ)
LIBDEP  = expat.$(SUFFIX_LIB)
KEYSTONE_LIB_DIR = $(KEYSTONE_DIR)/lib/$(KEYSTONE_TARGET)
KEYSTONE_TARGETS = keystone_lib 

endif
ifeq ($(KEYSTONE_TARGET), win32)
NATIVE_DIR   = win32
OBJS = $(OBJS_FILE) $(OBJS_BASE) 
GRLIBS  = graphics/win32/*.$(SUFFIX_OBJ)
GRLIBS_GTK = graphics/gtk/*.$(SUFFIX_OBJ)
LIBS    = graphics/*.$(SUFFIX_OBJ) expat/*.$(SUFFIX_OBJ) extras/*.$(SUFFIX_OBJ) zlib/zlib.$(SUFFIX_LIB)
LIBDEP  = expat.$(SUFFIX_LIB) zlib/zlib.$(SUFFIX_LIB)
RESOURCE_NATIVE = win32/keystone.res
KEYSTONE_LIB_DIR = $(KEYSTONE_DIR)/lib/$(KEYSTONE_TARGET)

ifdef KEYSTONE_WIN32_GTK
KEYSTONE_TARGETS = keystone_lib keystone_gtk_lib
else
KEYSTONE_TARGETS = keystone_lib
endif

endif

ifndef nograph
endif

all: $(OBJS) $(OBJS_KC) $(OBJS_NATIVE_MAIN) $(OBJS_NATIVE_MAIN) $(RESOURCE_NATIVE) frame_begin.$(SUFFIX_OBJ) frame_end.$(SUFFIX_OBJ) $(LIBDEP) $(KEYSTONE_TARGETS)
	cd .

keystone_lib:
	make -C graphics
	make -C extras
	$(KLIB) keystone.$(SUFFIX_LIB) $(OBJS) $(OBJS_KC) $(OBJS_NATIVE_MAIN) $(LIBS) $(GRLIBS) $(EXTRALIB)

keystone_gtk_lib:
	make -C graphics
	make -C extras
	$(KLIB) keystone_gtk.$(SUFFIX_LIB) $(OBJS) $(OBJS_KC) $(OBJS_NATIVE_MAIN) $(LIBS) $(GRLIBS_GTK) $(EXTRALIB)

tools_build:
	make -C tools

tools_install:
	make -C tools install

tools_remove:
	make -C tools remove

ifeq ($(KEYSTONE_TARGET), win32)
ifdef KEYSTONE_WIN32_GTK
install:
	$(COPY) frame_begin.$(SUFFIX_OBJ) $(KEYSTONE_LIB_DIR)
	$(COPY) frame_end.$(SUFFIX_OBJ) $(KEYSTONE_LIB_DIR)
	$(COPY) win32/keystone.res $(KEYSTONE_LIB_DIR)
	$(COPY) keystone.$(SUFFIX_LIB) $(KEYSTONE_LIB_DIR)
	$(COPY) keystone_gtk.$(SUFFIX_LIB) $(KEYSTONE_LIB_DIR)	
else
install:
	$(COPY) frame_begin.$(SUFFIX_OBJ) $(KEYSTONE_LIB_DIR)
	$(COPY) frame_end.$(SUFFIX_OBJ) $(KEYSTONE_LIB_DIR)
	$(COPY) win32/keystone.res $(KEYSTONE_LIB_DIR)	
	$(COPY) keystone.$(SUFFIX_LIB) $(KEYSTONE_LIB_DIR)
endif
else
install:
	$(COPY) frame_begin.$(SUFFIX_OBJ) $(KEYSTONE_LIB_DIR)
	$(COPY) frame_end.$(SUFFIX_OBJ) $(KEYSTONE_LIB_DIR)
	$(COPY) keystone.$(SUFFIX_LIB) $(KEYSTONE_LIB_DIR)
endif

expat.$(SUFFIX_LIB):
	make -C expat
	
zlib/zlib.$(SUFFIX_LIB):
	make -C zlib

clean:
	$(CLEAN) $(KEYSTONE_LIB_DIR)/frame_begin.$(SUFFIX_OBJ)
	$(CLEAN) $(KEYSTONE_LIB_DIR)/frame_end.$(SUFFIX_OBJ)
	$(CLEAN) $(KEYSTONE_LIB_DIR)/*.$(SUFFIX_LIB)
	make -C tools remove
	make -C tools clean
	make -C graphics clean
	make -C expat clean
	make -C zlib clean	
	make -C extras clean
	$(CLEAN) *.$(SUFFIX_LIB) *.$(SUFFIX_OBJ) base/*.$(SUFFIX_OBJ) $(EXTRACLEAN)

base/contain.$(SUFFIX_OBJ): base/contain.c
	$(KC) base/contain.c -o$@

base/exception.$(SUFFIX_OBJ): base/exception.c
	$(KC) base/exception.c -o$@

base/text.$(SUFFIX_OBJ): base/text.c
	$(KC) base/text.c -o$@

cencode.$(SUFFIX_OBJ): cencode.c
	$(CC) cencode.c -o$@
	
cdecode.$(SUFFIX_OBJ): cdecode.c
	$(CC) cdecode.c -o$@
	
datetime.$(SUFFIX_OBJ): datetime.c
	$(KC) datetime.c -o$@

object.$(SUFFIX_OBJ): object.c
	$(KC) object.c -o$@

objcontainer.$(SUFFIX_OBJ): objcontainer.c
	$(KC) objcontainer.c -o$@

objdataserver.$(SUFFIX_OBJ): objdataserver.c
	$(KC) objdataserver.c -o$@

objpersistent.$(SUFFIX_OBJ): objpersistent.c
	$(KC) objpersistent.c -o$@

objpersistent_xml.$(SUFFIX_OBJ): objpersistent_xml.c
	$(KC) objpersistent_xml.c -o$@

event.$(SUFFIX_OBJ): event.c
	$(KC) event.c -o$@

file.$(SUFFIX_OBJ): file.c
	$(KC) file.c -o$@

frame_begin.$(SUFFIX_OBJ): frame_begin.c
	$(KC) frame_begin.c -o$@

frame_end.$(SUFFIX_OBJ): frame_end.c
	$(KC) frame_end.c -o$@

main.$(SUFFIX_OBJ): main.c
	$(KC) main.c -o$@

framework_base.$(SUFFIX_OBJ): framework_base.c
	$(KC) framework_base.c -o$@

main_native.$(SUFFIX_OBJ): $(NATIVE_DIR)/main_native.c
	$(KC) $(NATIVE_DIR)/main_native.c -o$@

main_native_NOMAIN.$(SUFFIX_OBJ): $(NATIVE_DIR)/main_native.c
	$(KC) -DFRAMEWORK_NOMAIN $(NATIVE_DIR)/main_native.c -omain_native_NOMAIN.$(SUFFIX_OBJ)

memfile.$(SUFFIX_OBJ): memfile.c
	$(KC) memfile.c -o$@

serverstream.$(SUFFIX_OBJ): serverstream.c
	$(KC) serverstream.c -o$@

cstring.$(SUFFIX_OBJ): cstring.c
	$(KC) cstring.c -o$@

syncobject.$(SUFFIX_OBJ): syncobject.c
	$(KC) syncobject.c -o$@

socket.$(SUFFIX_OBJ): socket.c
	$(KC) socket.c -o$@

thread.$(SUFFIX_OBJ): $(NATIVE_DIR)/thread.c
	$(KC) $(NATIVE_DIR)/thread.c -o$@

#should really be a makefile in win32 dir
$(NATIVE_DIR)/keystone.res: $(NATIVE_DIR)/keystone.rc
	rc win32/keystone.rc

$(KEYSTONE_DIR)/doc/src/:
	attrib -r $(KEYSTONE_DIR)/doc/src/*
	rm -r -f $(KEYSTONE_DIR)/doc/src/
	"mkdir.exe" $(KEYSTONE_DIR)/doc/src/
	cp container.c $(KEYSTONE_DIR)/doc/src

.PHONY : doc
doc: $(KEYSTONE_DIR)/doc/src/ $(OBJS:%.$(SUFFIX_OBJ)=%.doc)
	make -C graphics doc
	make -C extras doc   
