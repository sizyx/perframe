#Makefile for perf_bench
#Created by sizyx@163.com
.SUFFIXES:
.SUFFIXES:  .c .o

CXX ?= g++
CC ?= gcc
EXE=perf_bench
EXTLIB=../perframe/libperframe.a
CFLAGS=-pthread -fPIC -fvisibility=hidden -I. -I../perframe/src 

ifeq ($(Mode), debug)
CFLAGS += -g -O0
endif

OBJS=perf_benchmark.o

all:$(EXE)

DEPS=$(OBJS:.o=.d)

-include $(DEPS)

$(EXE):$(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(EXTLIB) -ldl -pthread
	@rm -rf $(OBJS)
	@mv $(EXE) ../

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ -Wall -MD -MF $*.d -MP

clean:
	@rm -rf $(OBJS) $(EXE)
