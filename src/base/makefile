include ../rules.mak

OBJS    = contain.$(OBJ) text.$(OBJ) exception.$(OBJ)

all: $(OBJS)

clean:
	$(CLEAN) *.$(LIB) *.$(OBJ) $(EXTRACLEAN)

#template implementation

contain.$(OBJ): contain.c
	keycc contain.c -o$@

exception.$(OBJ): exception.c
	keycc exception.c -o$@

text.$(OBJ): text.c
	keycc text.c -o$@
