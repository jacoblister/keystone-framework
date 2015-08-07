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

ifeq ($(KEYSTONE_TARGET), win32)
#disable activex support for now 
NATIVE_EXTRA = gactivexcontainer.$(SUFFIX_OBJ) activex.$(SUFFIX_OBJ)
NATIVE_SQLITE_OPT = -DINVALID_FILE_ATTRIBUTES=((DWORD)-1)
endif

RESOURCE_OBJS = sqltest.svg

EXTRA_OBJ = sqlite3.$(SUFFIX_OBJ) sqlconn.$(SUFFIX_OBJ)

OBJS  = ggrid.$(SUFFIX_OBJ) sqlclient.$(SUFFIX_OBJ) $(EXTRA_OBJ) resource.$(SUFFIX_OBJ)

DOCOBJS  = ggrid.$(SUFFIX_OBJ) sqlclient.$(SUFFIX_OBJ) sqlconn.$(SUFFIX_OBJ)

#>>>dummy rule 'cd .' don't like this
all: $(OBJS) $(NATIVE_OBJS) $(NATIVE_EXTRA)
	cd .

clean:
	$(CLEAN) *.$(SUFFIX_OBJ) *.exe $(EXTRACLEAN) resource.c
   
.PHONY : doc
doc: $(DOCOBJS:%.$(SUFFIX_OBJ)=%.doc)
	cd .

gactivexcontainer.$(SUFFIX_OBJ): gactivexcontainer.c $(HEADERS)
	$(KC) gactivexcontainer.c /Fo$@
    
activex.$(SUFFIX_OBJ): activex.cpp $(HEADERS)
	cl -c /Yd /Zi activex.cpp /Fo$@
  
ggrid.$(SUFFIX_OBJ): ggrid.c
	$(KC) ggrid.c /Fo$@

sqlclient.$(SUFFIX_OBJ): sqlclient.c
	$(KC) sqlclient.c -o$@ $(EXTRA_OPTION)
   
sqlconn.$(SUFFIX_OBJ): sqlconn.c
	$(KC) sqlconn.c -o$@

sqlite3.$(SUFFIX_OBJ): sqlite3.c
	$(CC) sqlite3.c -DSQLITE_THREADSAFE=0 $(NATIVE_SQLITE_OPT) -o$@
 
resource.$(SUFFIX_OBJ): resource.c
	$(KC) resource.c -o$@
resource.c: $(RESOURCE_OBJS)
	$(KFILE) resource.c $(RESOURCE_OBJS) 