# 2. projekt do KRY
# Implementace a prolomeni RSA 
#
# autor: Jan Wozniak

PROJ   = kry
SRC    = src/
FLAGS  = -std=c99 -Wall -pedantic -w -Isrc/msieve/include
CC     = gcc
LIBS   = -lgmp -lm -lpthread 
MSIEVE = src/msieve/libmsieve.a


$(PROJ): $(SRC)main.c 
	cd src/msieve && make all && cd -
	$(CC) $(FLAGS) $(SRC)main.c -o $(PROJ) $(LIBS) $(MSIEVE) 

run: 
	./kry -g 8

pack:
	tar -zcvf xwozni00.tar.gz src/* Makefile dokumentace.pdf

clean:
	cd src/msieve && make clean && cd -
	rm -rf kry
	rm -rf *.o
