VERSION_0	 = 0
VERSION_1	 = 9
VERSION_2	 = 6
VERSION_3	 = 24

.PHONY : framework
framework: tools
	printf "#define VERSION_NUMBER %%s,%%s,%%s,%%s\n#define VERSION_STRING \"%%s.%%s.%%s-%%s%%s\"\n" \
	$(VERSION_0) $(VERSION_1) $(VERSION_2) $(VERSION_3) $(VERSION_0) $(VERSION_1) $(VERSION_2) $(VERSION_3) $(VERSION_OPTION_STR) >version.h
	make -C src
	make -C src install
   
.PHONY : tools
tools:
	make -C src tools_build
	make -C src tools_install

.PHONY : clean
clean:
	make -C src clean
	make -C examples clean
	make -C doc clean

.PHONY : examples
examples: tools
	make -C examples

.PHONY : doc
doc: tools
	make -C src doc
	make -C doc doc

.PHONY : arch
arch: clean
	chmod +rw -R *.*
	rm -f *.gz *.tar
	tar -c -fkeystone.tar *
	gzip keystone.tar