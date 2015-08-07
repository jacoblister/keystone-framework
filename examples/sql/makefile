# Keystone Pre Alpha Release
#
# by Jacob Lister - 2002.
#
# contact j_lister@paradise.net.nz
#
# for use by Abbey Systems Wellington internally only
#
# DO NOT DISTRIBUTE

include /home/jacob/keystone/src/rules.mak

all: sqltest #sqlclient sqlserver 

sqltest: sqltest.$(SUFFIX_OBJ) 
	$(KLINK) -osqltest sqltest.$(SUFFIX_OBJ) 
   
sqlclient: client.$(SUFFIX_OBJ) 
	$(KLINK) -osqlclient client.$(SUFFIX_OBJ) ws2_32.lib
   
sqlserver: server.c
	$(CCOMPILER) /Yd /Zi server.c -osqlserver ..\..\src\extras\sqlconn.$(SUFFIX_OBJ) ..\..\src\extras\sqlite3.$(SUFFIX_OBJ) ws2_32.lib 

clean:
	rm --force *.$(SUFFIX_OBJ) *.ilk *.pdb *.cm *.exe
	rm --force sqlclient

sqltest.$(SUFFIX_OBJ): sqltest.c
	$(KC) sqltest.c -o$@
   
client.$(SUFFIX_OBJ): client.c
	$(KC) client.c -o$@
