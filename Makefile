SUBDIRS=perframe benchmark

.PHONY:all clean

all:
	@for dir in $(SUBDIRS); do make -C $$dir; done

clean:
	@for dir in $(SUBDIRS); do make -C $$dir clean; done