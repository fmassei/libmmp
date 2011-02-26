lib_LTLIBRARIES = libmmp_base.la
libmmp_base_la_SOURCES = mmp_date.c mmp_error.c mmp_files.c mmp_funclist.c \
						 mmp_getopt.c mmp_memory.c mmp_mmap.c mmp_shm.c \
						 mmp_string.c mmp_system.c mmp_tap.c mmp_trace.c
libmmpdir = $(includedir)/mmp
libmmp_HEADERS =         mmp_date.h mmp_error.h mmp_files.h mmp_funclist.h \
						 mmp_getopt.h mmp_memory.h mmp_mmap.h mmp_shm.h \
						 mmp_string.h mmp_system.h mmp_tap.h mmp_trace.h \
				  		 mmp_compat.h mmp_h_utils.h
if WITHCONTAINERS
libmmp_base_la_SOURCES += mmp_2tree.c mmp_barray.c mmp_btree.c mmp_dir.c \
						  mmp_htab.c mmp_list.c mmp_stack.c mmp_table.c
libmmp_HEADERS +=         mmp_2tree.h mmp_barray.h mmp_btree.h mmp_dir.h \
						  mmp_htab.h mmp_list.h mmp_stack.h mmp_table.h
endif
if WITHTHREADS
libmmp_base_la_SOURCES += mmp_thread.c mmp_sync.c
libmmp_HEADERS +=         mmp_thread.h mmp_sync.h
endif
if WITHDL
libmmp_base_la_SOURCES += mmp_dl.c
libmmp_HEADERS +=         mmp_dl.h
endif
if WITHSOCKETS
libmmp_base_la_SOURCES += mmp_socket.c
libmmp_HEADERS +=         mmp_socket.h
endif

libmmp_base_la_LDFLAGS = -version-info 0:0:0
libmmp_base_la_CFLAGS = $(AM_CFLAGS)

check_PROGRAMS = test_stack test_table
test_stack_SOURCES = test_mmp_stack.c
test_stack_LDFLAGS = -lmmp_base
test_table_SOURCES = test_mmp_table.c
test_table_LDFLAGS = -lmmp_base
TESTS = $(check_PROGRAMS)