CC=/usr/local/common/gcc-4.9.0/bin/gcc
CFLAGS=--std=c99 -D_GNU_SOURCE -Wall -Werror -g
LDFLAGS=-Wl,-rpath,/usr/local/common/gcc-4.9.0/lib64/

all: common.o object.o strfun.o readwrite.o argparse.o argparse_iforest.o frames.o

test.exe: common.o object.o strfun.o readwrite.o argparse.o argparse_iforest.o frames.o test.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o test.exe test.o common.o object.o strfun.o readwrite.o argparse.o argparse_iforest.o frames.o

%.o: %.c %.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $<

test: test.exe
	./test.exe -ivHm test.csv 1-2 --header=true --outfile=potahto

fresh:
	make clean
	make all

clean:
	rm -rf *.o*
	rm -rf *.exe
