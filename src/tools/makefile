include ../rules.mak

ifeq ($(KEYSTONE_TARGET), linux)
	EXE		=
	KEYSTONE_BIN_DIR = $(KEYSTONE_DIR)/bin/$(KEYSTONE_TARGET)
endif
ifeq ($(KEYSTONE_TARGET), win32)
	EXE		= .exe
	KEYSTONE_BIN_DIR = $(KEYSTONE_DIR)\bin\$(KEYSTONE_TARGET)
endif

KEYCC_MODULES = keycc.$(SUFFIX_OBJ) configparser.$(SUFFIX_OBJ) combine.$(SUFFIX_OBJ) template_module.$(SUFFIX_OBJ) template_container.$(SUFFIX_OBJ) \
	template_class.$(SUFFIX_OBJ) template_exception.$(SUFFIX_OBJ) template_text.$(SUFFIX_OBJ)

all: keycc keylib keylink file2module

install:
	$(COPY) keycc$(EXE)       $(KEYSTONE_BIN_DIR)
	$(COPY) keylib$(EXE)      $(KEYSTONE_BIN_DIR)
	$(COPY) keylink$(EXE)     $(KEYSTONE_BIN_DIR)
	$(COPY) file2module$(EXE) $(KEYSTONE_BIN_DIR)

remove:
	$(CLEAN) $(KEYSTONE_BIN_DIR)/keycc$(EXE)
	$(CLEAN) $(KEYSTONE_BIN_DIR)/keylib$(EXE)
	$(CLEAN) $(KEYSTONE_BIN_DIR)/keylink$(EXE)
	$(CLEAN) $(KEYSTONE_BIN_DIR)/file2module$(EXE)

keycc: $(KEYCC_MODULES)
	$(LINK) $(KEYCC_MODULES) $(EXE_OUT)keycc $(LINKOPTION)

keylib: keylib.$(SUFFIX_OBJ) configparser.$(SUFFIX_OBJ)
	$(LINK) keylib.$(SUFFIX_OBJ) configparser.$(SUFFIX_OBJ) $(EXE_OUT)keylib $(LINKOPTION)

keylink: keylink.$(SUFFIX_OBJ) configparser.$(SUFFIX_OBJ)
	$(LINK) keylink.$(SUFFIX_OBJ) configparser.$(SUFFIX_OBJ) $(EXE_OUT)keylink $(LINKOPTION)

file2module: file2module.$(SUFFIX_OBJ)
	$(LINK) file2module.$(SUFFIX_OBJ) $(EXE_OUT)file2module $(LINKOPTION)

file2module.$(SUFFIX_OBJ): file2module.c
	$(CC) file2module.c $(OBJ_OUT)$@

clean:
	$(CLEAN) *.$(SUFFIX_LIB) *.$(SUFFIX_OBJ) $(EXTRACLEAN)

keycc.$(SUFFIX_OBJ): keycc.c
	$(CC) keycc.c $(OBJ_OUT)$@

keylib.$(SUFFIX_OBJ): keylib.c
	$(CC) keylib.c $(OBJ_OUT)$@

keylink.$(SUFFIX_OBJ): keylink.c
	$(CC) keylink.c $(OBJ_OUT)$@

combine.$(SUFFIX_OBJ): combine.c
	$(CC) combine.c $(OBJ_OUT)$@

configparser.$(SUFFIX_OBJ): configparser.c
	$(CC) configparser.c $(OBJ_OUT)$@

template_module.$(SUFFIX_OBJ): template_module.c
	$(CC) template_module.c $(OBJ_OUT)$@

template_container.$(SUFFIX_OBJ): template_container.c
	$(CC) template_container.c $(OBJ_OUT)$@

template_class.$(SUFFIX_OBJ): template_class.c
	$(CC) template_class.c $(OBJ_OUT)$@

template_exception.$(SUFFIX_OBJ): template_exception.c
	$(CC) template_exception.c $(OBJ_OUT)$@

template_text.$(SUFFIX_OBJ): template_text.c
	$(CC) template_text.c $(OBJ_OUT)$@
