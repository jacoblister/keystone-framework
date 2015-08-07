#\brief Keystone source file
#
#Keystone Pre Alpha Release
#
#by Jacob Lister - 2004.
#
#contact jacob@keystoneframework.org
#

KEYSTONE_DIR = /home/carbon/keystone
KEYSTONE_TARGET = linux
KEYSTONE_WIN32_GDIP = TRUE
NULL   =

ifeq ($(KEYSTONE_TARGET), linux)
	TARGET		= -DOS_Linux -DCOMP_GCC
	CCOMPILER	= gcc
	CLEAN		= rm --force
	EXTRACLEAN	= *~ base/*~ *.cm
	OBJ_OUT		= -o
	EXE_OUT		= -o 
	COPY    = cp
	SUFFIX_OBJ=o
	SUFFIX_LIB=a
	DEBUG		=
endif
ifeq ($(KEYSTONE_TARGET), win32)
	SHELL		= cmd.exe
	TARGET		= -DOS_Win32 -DCOMP_MSVC -D_CRT_SECURE_NO_DEPRECATE
	DEBUG		= /Zi
#	DEBUG		= /O2 /Ot
	CCOMPILER	= cl
	CLEAN		= rm --force
	EXTRACLEAN	= *.ilk *.pdb *.cm
	OBJ_OUT		= /Fo
	EXE_OUT		= /Fe
ifdef KEYSTONE_WIN32_GDIP	
	EXTRALIB 	= winspool.lib gdi32.lib user32.lib comctl32.lib comdlg32.lib delayimp.lib gdiplus.lib 
else
	EXTRALIB 	= gdi32.lib winspool.lib user32.lib comctl32.lib comdlg32.lib 
endif	
	EXE			= .exe
	COPY    	= cp
	SUFFIX_OBJ=obj
	SUFFIX_LIB=lib
	NORDONLY = attrib -r 
endif
ifeq ($(KEYSTONE_TARGET), palmos)
#	NATIVE_LIBS     =
	TARGET		= -DOS_PalmOS -DCOMP_M68K_PALMOS_GCC
	CCOMPILER	= m68k-palmos-gcc
	SYSFLAGS	= -Wall
	OPT		= -O3 -fno-inline
	DEBUG		= -g
	CLEAN		= rm --force
	ETRACLEAN	= *~ base/*~
	OBJ_OUT		= -o
	LIB		= a
	MAKELIB		= m68k-palmos-ar rvs $(NULL)
	OBJ		= o
	LINKAPP		= -T palmos.x
endif

LINK	   = $(CCOMPILER) $(DEBUG)

ifdef test
TEST        = -DTEST
endif
CFLAGS      = $(TARGET) -I$(KEYSTONE_DIR)/src/base -c
CC          = $(CCOMPILER) $(CFLAGS) $(TEST) $(DEBUG)
KC          = $(KEYSTONE_DIR)/bin/$(KEYSTONE_TARGET)/keycc $(TEST)
KLIB        = $(KEYSTONE_DIR)/bin/$(KEYSTONE_TARGET)/keylib
KLINK       = $(KEYSTONE_DIR)/bin/$(KEYSTONE_TARGET)/keylink
KFILE       = $(KEYSTONE_DIR)/bin/$(KEYSTONE_TARGET)/file2module
DOC         = keycc --noinclude --documentation $< > $(KEYSTONE_DIR)/doc/src/$<
#DOC         = cp $< $(KEYSTONE_DIR)/doc/src/$<

%.doc : %.c
	$(DOC)

