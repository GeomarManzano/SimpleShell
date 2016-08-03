# Author: Geomar Manzano

CC = gcc
OPTIMIZE = -O2
CFLAGS = $(OPTIMIZE) -Wall
DISTDIR = lab1-geomarmanzano

all: simpsh

stack.o: stack.c
	$(CC) $(CFLAGS) -c $^

queue.o: queue.c
	$(CC) $(CFLAGS) -c $^

handler.o: handler.c
	$(CC) $(CFLAGS) -c $^

options.o: options.c
	$(CC) $(CFLAGS) -c $^

simpsh.o: simpsh.c
	$(CC) $(CFLAGS) -c $^

simpsh: stack.o queue.o handler.o options.o simpsh.o
	$(CC) $(CFLAGS) stack.o queue.o handler.o options.o simpsh.o -o \
	simpsh -lreadline

DIST_SOURCES = README Makefile report.pdf \
	stack.c queue.c handler.c options.c simpsh.c \
	handler.h queue.h options.h stack.h test.sh foo oof bar numbers words \
	FOOBAR_BASH sedTest sortTest catTest 

dist: $(DISTDIR).tar.gz

$(DISTDIR).tar.gz: $(DIST_SOURCES)
	rm -fr $(DISTDIR)
	tar -czf $@.tmp --transform='s,^,$(DISTDIR)/,' $(DIST_SOURCES)
	mv $@.tmp $@

check:
	./test.sh

.PHONY: all clean dist

clean:
	rm -rf *.o simpsh lab1-geomarmanzano.tar.gz FOO FOOBAR_SIMPSH foo_* lab1-geomarmanzano/
