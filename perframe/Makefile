#Makefile for libperframe.a
#Created by sizyx@163.com
.SUFFIXES:
.SUFFIXES:  .c .o

CC ?= gcc
STATLICLIB=libperframe.a
CFLAGS=-pthread -fPIC -fvisibility=hidden -DHAVE_CONFIG_H -DLINUX -D_REENTRANT -D_GNU_SOURCE -I. -Iapr/include -Iapr/include/arch/unix -Iapr/include/private -c

ifeq ($(Mode), debug)
CFLAGS += -g -O0
endif

OBJS=\
apr/encoding/apr_encode.o \
apr/encoding/apr_escape.o \
apr/passwd/apr_getpass.o \
apr/strings/apr_cpystrn.o \
apr/strings/apr_cstr.o \
apr/strings/apr_fnmatch.o \
apr/strings/apr_snprintf.o \
apr/strings/apr_strings.o \
apr/strings/apr_strnatcmp.o \
apr/strings/apr_strtok.o \
apr/tables/apr_hash.o \
apr/tables/apr_skiplist.o \
apr/tables/apr_tables.o \
apr/atomic/unix/builtins.o \
apr/atomic/unix/builtins64.o \
apr/atomic/unix/ia32.o \
apr/atomic/unix/mutex.o \
apr/atomic/unix/mutex64.o \
apr/atomic/unix/ppc.o \
apr/atomic/unix/s390.o \
apr/atomic/unix/solaris.o \
apr/dso/unix/dso.o \
apr/file_io/unix/buffer.o \
apr/file_io/unix/copy.o \
apr/file_io/unix/dir.o \
apr/file_io/unix/fileacc.o \
apr/file_io/unix/filedup.o \
apr/file_io/unix/filepath.o \
apr/file_io/unix/filepath_util.o \
apr/file_io/unix/filestat.o \
apr/file_io/unix/flock.o \
apr/file_io/unix/fullrw.o \
apr/file_io/unix/mktemp.o \
apr/file_io/unix/open.o \
apr/file_io/unix/pipe.o \
apr/file_io/unix/readwrite.o \
apr/file_io/unix/seek.o \
apr/file_io/unix/tempdir.o \
apr/locks/unix/global_mutex.o \
apr/locks/unix/proc_mutex.o \
apr/locks/unix/thread_cond.o \
apr/locks/unix/thread_mutex.o \
apr/locks/unix/thread_rwlock.o \
apr/memory/unix/apr_pools.o \
apr/misc/unix/charset.o \
apr/misc/unix/env.o \
apr/misc/unix/errorcodes.o \
apr/misc/unix/getopt.o \
apr/misc/unix/otherchild.o \
apr/misc/unix/rand.o \
apr/misc/unix/start.o \
apr/misc/unix/version.o \
apr/mmap/unix/common.o \
apr/mmap/unix/mmap.o \
apr/network_io/unix/inet_ntop.o \
apr/network_io/unix/inet_pton.o \
apr/network_io/unix/multicast.o \
apr/network_io/unix/sendrecv.o \
apr/network_io/unix/sockaddr.o \
apr/network_io/unix/socket_util.o \
apr/network_io/unix/sockets.o \
apr/network_io/unix/sockopt.o \
apr/poll/unix/epoll.o \
apr/poll/unix/kqueue.o \
apr/poll/unix/poll.o \
apr/poll/unix/pollcb.o \
apr/poll/unix/pollset.o \
apr/poll/unix/port.o \
apr/poll/unix/select.o \
apr/poll/unix/wakeup.o \
apr/poll/unix/z_asio.o \
apr/random/unix/apr_random.o \
apr/random/unix/sha2.o \
apr/random/unix/sha2_glue.o \
apr/shmem/unix/shm.o \
apr/support/unix/waitio.o \
apr/threadproc/unix/proc.o \
apr/threadproc/unix/procsup.o \
apr/threadproc/unix/signals.o \
apr/threadproc/unix/thread.o \
apr/threadproc/unix/threadpriv.o \
apr/time/unix/time.o \
apr/time/unix/timestr.o \
apr/user/unix/groupinfo.o \
apr/user/unix/userinfo.o \
src/perframe_input.o \
src/perframe_main.o \
src/perframe_printf.o


all:$(STATLICLIB)

LIB_DEPS=$(OBJS:.o=.d)

-include $(LIB_DEPS)

$(STATLICLIB):$(OBJS)
	ar cr $(STATLICLIB) $(OBJS)
	ranlib $(STATLICLIB)

%.o:%.c
	$(CC) $(CFLAGS) $< -o $@ -Wall -MD -MF $*.d -MP

clean:
	rm -f $(STATLICLIB) $(OBJS) $(LIB_DEPS)
