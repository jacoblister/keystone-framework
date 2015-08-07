include ../rules.mak

INC     = $(FRAMEINC)

#XML_CFLAGS = -Ixmltok -Ixmlparse -DXML_NS -DXML_DTD
XML_CFLAGS = -Isrc -DXML_NS -DXML_DTD -D_STATIC -DHAVE_EXPAT_CONFIG_H

OBJS = xmltok.$(SUFFIX_OBJ) xmlrole.$(SUFFIX_OBJ) xmlparse.$(SUFFIX_OBJ)

#>>>dummy rule 'cd .' don't like this
all: $(OBJS)
	cd .

clean:
	$(CLEAN) *.$(SUFFIX_LIB) *.$(SUFFIX_OBJ) $(EXTRACLEAN)

gennmtab/gennmtab.exe: gennmtab/gennmtab.c
	$(CC) $(CFLAGS) -o $@ gennmtab/gennmtab.c

xmltok.$(SUFFIX_OBJ): src/xmltok.c $(HEADERS)
	$(CC) $(XML_CFLAGS) src/xmltok.c $(OBJ_OUT)$@

xmlrole.$(SUFFIX_OBJ): src/xmlrole.c $(HEADERS)
	$(CC) $(XML_CFLAGS) src/xmlrole.c $(OBJ_OUT)$@

xmlparse.$(SUFFIX_OBJ): src/xmlparse.c $(HEADERS)
	$(CC) $(XML_CFLAGS) src/xmlparse.c $(OBJ_OUT)$@
