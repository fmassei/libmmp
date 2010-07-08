GCC =   gcc
RM  =   rm -f
AR	=	ar rcs
GCCFLAG	= -Wall -std=c99 -pedantic -fPIC
LDFLAG	=
INCLUDE	= -I.
SRCDIR	= ..
OBJDIR	= .
BINDIR	= .
FINAME	= libmmp_base.a
TSTNAME = unittests

OBJS   = mmp_dl.o mmp_error.o mmp_trace.o mmp_list.o mmp_string.o mmp_dir.o \
    mmp_shm.o mmp_tap.o mmp_htab.o mmp_socket.o mmp_thread.o \
	mmp_sync.o mmp_files.o mmp_date.o mmp_getopt.o \
	unittest.o
OBJWD  = echo $(OBJDIR)/$(OBJS) | sed 's/\ /\ \.\//g'

base: all

%.o: $(SRCDIR)/%.c
	$(GCC) $(INCLUDE) $(GCCFLAG) -c $< -o $(OBJDIR)/$@

clean:
	-$(RM) $(OBJDIR)/*.o
	-$(RM) $(BINDIR)/$(FINAME)
	-$(RM) $(BINDIR)/$(TSTNAME)

all: $(OBJS)
	$(AR) $(FINAME) $(OBJS)

unittestscmp: $(OBJS)
	$(GCC) $(OBJS) -o $(BINDIR)/$(TSTNAME) -ldl -lrt

debug: GCCFLAG += -g
debug: all

unittests: GCCFLAG = -g -Wall -ansi -pedantic -DUNIT_TESTING
unittests: unittestscmp

