CC=gcc
CFLAGS=--std=c99 -D_GNU_SOURCE -Wall -Werror -g
PP=g++
PFLAGS=--std=c++11 -Wall -Werror -g  
all: iforest.exe
C/%.o: C/%.c C/%.h 
	$(CC) $(CFLAGS) -c $< -o $@
cincl.o: C/common.o C/object.o C/strfun.o C/readwrite.o C/argparse.o C/argparse_iforest.o C/frames.o
	ld -r C/common.o C/object.o C/strfun.o C/readwrite.o C/argparse.o C/argparse_iforest.o C/frames.o -o cincl.o
%.o: %.cpp %.hpp
	$(PP) $(PFLAGS) -c $< -o $@
iforest.exe: cincl.o Forest.o IsolationForest.o Tree.o utility.o OnlineIF.o main.o
	$(PP) $(PFLAGS) -o iforest.exe cincl.o Forest.o IsolationForest.o Tree.o utility.o OnlineIF.o main.o 
fresh:
	make clean
	make all
clean:
	rm -rf *.o*
	rm -rf *.exe
	rm -rf C/*.o*
	rm -rf C/*.exe
