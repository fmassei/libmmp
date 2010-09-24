GCC =   gcc
RM  =   rm -f
LN	=	ln -s
AR	=	ar rcs
STRIP = strip
INSTALL = install
GCCFLAG	= -Wall -std=c99 -pedantic -fPIC
LDFLAG	= -shared -Wl,-soname,$(LNNAME) -lc
INCLUDE	= -I.
SRCDIR	= ..
OBJDIR	= .
BINDIR	= .
FINAME  = libmmp_base.so.0.1
LNNAME	= libmmp_base.so.0
TSTNAME = unittests

OBJS   = mmp_dl.o mmp_error.o mmp_trace.o mmp_list.o mmp_string.o mmp_dir.o \
	mmp_shm.o mmp_tap.o mmp_htab.o mmp_socket.o mmp_thread.o \
	mmp_sync.o mmp_files.o mmp_date.o mmp_getopt.o mmp_barray.o \
	mmp_system.o mmp_mmap.o mmp_stack.o mmp_btree.o mmp_2tree.o \
	unittest.o
OBJWD  = echo $(OBJDIR)/$(OBJS) | sed 's/\ /\ \.\//g'

base: all

%.o: $(SRCDIR)/%.c
	$(GCC) $(INCLUDE) $(GCCFLAG) -c $< -o $(OBJDIR)/$@

clean:
	-$(RM) $(OBJDIR)/*.o
	-$(RM) $(BINDIR)/$(FINAME)
	-$(RM) $(BINDIR)/$(TSTNAME)

main: $(OBJS)
	$(GCC) $(shell $(OBJWD)) $(LDFLAG) -o $(BINDIR)/$(FINAME)

all: main
	$(STRIP) $(FINAME)

unittestscmp: $(OBJS)
	$(GCC) $(shell $(OBJWD)) -o $(BINDIR)/$(TSTNAME) -ldl -lrt

debug: GCCFLAG += -g
debug: main

install:
	$(INSTALL) -m 755 -d /usr/include/mmp
	$(INSTALL) -m 644 $(SRCDIR)/*.h /usr/include/mmp
	$(INSTALL) -m 644 $(FINAME) /usr/lib/
	$(LN) $(FINAME) $(LNNAME)

unittests: GCCFLAG = -g -Wall -std=c99 -pedantic -DUNIT_TESTING
unittests: unittestscmp

